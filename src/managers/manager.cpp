#include "manager.hpp"

#include "src/command/set_particles.hpp"
#include "src/command/copy_coordinates.hpp"
#include "src/command/ionize_particles.hpp"
#include "src/command/set_Bz_distribution.hpp"
#include "src/command/magnetic_field_half_step.hpp"

#include "src/fields/fields_builder.hpp"
#include "src/particles/particles_builder.hpp"
#include "src/diagnostics/diagnostics_builder.hpp"

#include "src/particles/particles_load.hpp"
#include "src/utils/random_number_generator.hpp"
#include "src/utils/simulation_backup.hpp"


void Manager::log_information() const {
  LOG_INFO("Note: Dimensionless units are used. So, for density 1e13:");
  LOG_INFO("  frequency,   w_pe = {:.2e} 1/sec", 5.64e4 * sqrt(1e13));
  LOG_INFO("  time,      1/w_pe = {:.2e} sec", 1.77e-5 / sqrt(1e13));
  LOG_INFO("  length,    c/w_pe = {:.2e} cm", 531915 / sqrt(1e13));
  LOG_INFO("  electric field, E = {:.2e} MV/cm", 9.63e-7 * sqrt(1e13));
  LOG_INFO("  magnetic field, B = {:.2e} T", 3.21e-7 * sqrt(1e13));

  // Commenting useful information about the current setup
  LOG_INFO("Constants for the current setup...");

  LOG_INFO("Simulation area:");
  LOG_INFO("  length along x axis,  Lx = {:.2e} c/w_pe ({} cells)", SIZE_X * dx, SIZE_X);
  LOG_INFO("  length along y axis,  Ly = {:.2e} c/w_pe ({} cells)", SIZE_Y * dy, SIZE_Y);
  LOG_INFO("  total simulation time, T = {:.2e} 1/w_pe ({} timesteps)", TIME * dt, TIME);
  // LOG_INFO("  magnetic field,    B_max = {:.2e} m_e*c*w_pe/e", config::Omega_max);

  LOG_FLUSH();
}


void Manager::initializes() {
  PROFILE_FUNCTION();

  Manager::log_information();
  LOG_TRACE("Initialization process...");

  std::list<Command_up> presets;

  Fields_builder fields_builder;
  fields_ = Fields(fields_builder);

  // We use this to push the magnetic field to
  // the same half-timestep as electric field
  step_presets_.push_front(std::make_unique<Magnetic_field_half_step>(&fields_));

  presets.push_back(std::make_unique<Set_Bz_distribution>(&fields_));

  Particles_builder particles_builder(fields_);

  Domain_geometry domain {
    config::domain_r_min,
    config::domain_r_max,
    config::domain_phi_min,
    config::domain_phi_max
  };

  std::unordered_map<std::string, Particles&>{
    // ???
  },

  particles_builder.set_sort("plasma_electrons");
  Particles& plasma_electrons = particles_species_.emplace_back(particles_builder);

  plasma_electrons.boundaries_processor_ = std::make_unique<POL_Beam_boundary>(
    plasma_electrons.particles_, plasma_electrons.parameters_, domain);

  plasma_electrons.particles_.reserve(config::PER_STEP_PARTICLES * TIME + 10'000);


#if MAKE_BACKUPS || START_FROM_BACKUP
  auto backup =  std::make_unique<Simulation_backup>(
    /* backup timestep = */ 100 * diagnose_time_step,
    // named particle species to backup:
    std::unordered_map<std::string, Particles&>{
      // ???
    },
    // named fields to backup:
    std::unordered_map<std::string, vector3_field&>{
      { "E", fields_.E() },
      { "B", fields_.B() }
  });

#endif

#if !START_FROM_BACKUP
  for (const auto& command : presets) {
    command->execute(START_);
  }

#else
  backup->load();
  START_ = backup->get_last_timestep();
  LOG_INFO("Configuration loaded from backup. Simulation will start from t={}", START_);

#endif

  Diagnostics_builder diagnostics_builder(particles_species_, fields_);
  diagnostics_ = diagnostics_builder.build();

#if MAKE_BACKUPS
  diagnostics_.emplace_back(std::move(backup));
#endif

  diagnose(START_);
  LOG_FLUSH();
}


void Manager::calculates() {
  for (size_t t = START_ + 1u; t <= TIME; ++t) {
    LOG_TRACE("------------------------------------ one timestep ------------------------------------");
    PROFILE_SCOPE("one timestep");

  #if there_are_fields
    fields_.clear_sources();
  #endif

    for (auto& command : step_presets_) {
      command->execute(t);
    }

  #if there_are_particles
    for (auto& sort : particles_species_) {
      sort.push();
    }
  #endif

  #if there_are_fields
    fields_.propagate();
  #endif

    diagnose(t);

    step_presets_.remove_if([t](const Command_up& command) {
      return command->needs_to_be_removed(t);
    });
  }
}


void Manager::diagnose(size_t t) const {
  PROFILE_FUNCTION();

  #pragma omp parallel for shared(diagnostics_), num_threads(NUM_THREADS)
  for (const auto& diagnostic : diagnostics_) {
    diagnostic->diagnose(t);
  }
}
