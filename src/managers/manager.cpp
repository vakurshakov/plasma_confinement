#include "manager.hpp"

#include "src/utils/configuration.hpp"
#include "src/utils/simulation_backup.hpp"

#include "src/fields/fields_builder.hpp"
#include "src/particles/particles_builder.hpp"
#include "src/diagnostics/diagnostics_builder.hpp"
#include "src/command/commands_builder.hpp"


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

  /// @todo Add usage description into runtime exception
  /// @todo Add a wraparound for "Initialization error"
  Fields_builder fields_builder;
  fields_ = Fields(fields_builder);

  Particles_builder particles_builder(fields_);
  particles_species_ = particles_builder.build();

  Commands_builder commands_builder(particles_species_, fields_);
  step_presets_ = commands_builder.build_step_presets();
  auto presets = commands_builder.build_presets();

  Diagnostics_builder diagnostics_builder(particles_species_, fields_);
  diagnostics_ = diagnostics_builder.build();

#if MAKE_BACKUPS || START_FROM_BACKUP
  /// @todo specify backup_time_step via config file
  auto backup =  std::make_unique<Simulation_backup>(
    /* backup timestep = */ 100 * diagnose_time_step,
    particles_species_,
    // named fields to backup:
    std::map<std::string, vector3_field&>{
      std::pair<std::string, vector3_field&>{"E", fields_.E()},
      std::pair<std::string, vector3_field&>{"B", fields_.B()}
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

#if MAKE_BACKUPS
  diagnostics_.emplace_back(std::move(backup));
#endif

  diagnose(START_);
  LOG_FLUSH();
}


void Manager::calculates() {
  for (int t = START_ + 1; t <= TIME; ++t) {
    LOG_TRACE("------------------------------------ one timestep ------------------------------------");
    PROFILE_SCOPE("one timestep");

  #if THERE_ARE_FIELDS
    fields_.clear_sources();
  #endif

    for (auto& command : step_presets_) {
      command->execute(t);
    }

  #if THERE_ARE_PARTICLES
    for (auto& sort : particles_species_) {
      sort.push();
    }
  #endif

  #if THERE_ARE_FIELDS
    fields_.propagate();
  #endif

    diagnose(t);

    step_presets_.remove_if([t](const Command_up& command) {
      return command->needs_to_be_removed(t);
    });
  }
}


void Manager::diagnose(int t) const {
  PROFILE_FUNCTION();

  #pragma omp parallel for shared(diagnostics_), num_threads(OMP_NUM_THREADS)
  for (const auto& diagnostic : diagnostics_) {
    diagnostic->diagnose(t);
  }
}
