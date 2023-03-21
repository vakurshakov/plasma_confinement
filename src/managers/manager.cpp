#include "manager.hpp"

#include "src/command/set_particles.hpp"
#include "src/command/copy_coordinates.hpp"
#include "src/command/ionize_particles.hpp"
#include "src/command/clone_layer_particles.hpp"
#include "src/command/set_Bz_distribution.hpp"
#include "src/command/magnetic_field_half_step.hpp"

#include "src/fields/fields_builder.hpp"
#include "src/particles/particles_builder.hpp"
#include "src/particles/particles_load.hpp"
#include "src/diagnostics/diagnostics_builder.hpp"

#include "src/utils/transition_layer/particles_distribution.hpp"
static auto __n = Table_function("src/utils/transition_layer/n_" + config::postfix);

#include "src/utils/random_number_generator.hpp"
#include "src/utils/simulation_backup.hpp"

void Manager::initializes() {
  PROFILE_FUNCTION();

  // Common information
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
  LOG_INFO("  magnetic field,    B_max = {:.2e} m_e*c*w_pe/e", config::Omega_max);

  LOG_INFO("Plasma ions:");
  LOG_INFO("  temperature,              T_i = {:.2e} KeV", config::T_ions);
  LOG_INFO("  thermal velocity,        v_Ti = {:.2e} c", config::V_ions);
  LOG_INFO("  Debye length,            L_di = {:.2e} c/w_pe", config::V_ions * sqrt(config::mi_me));
  LOG_INFO("  cyclotron frequency,     Om_i = {:.2e} w_pe", config::Omega_max / config::mi_me);
  LOG_INFO("  cyclotron period, 2 pi / Om_i = {:.2e} 1/w_pe ({} timesteps)",
    2 * M_PI / (config::Omega_max / config::mi_me),
    int(2 * M_PI / (config::Omega_max / config::mi_me) / dt));
  LOG_INFO("  cyclotron radius,       rho_i = {:.2e} c/w_pe ({} cells)",
    config::V_ions * config::mi_me / config::Omega_max,
    int(config::V_ions * config::mi_me / config::Omega_max / dx));

  LOG_INFO("Plasma electrons:");
  LOG_INFO("  temperature,              T_e = {:.2e} KeV", config::T_electrons);
  LOG_INFO("  thermal velocity,        v_Te = {:.2e} c", config::V_electrons);
  LOG_INFO("  Debye length,            L_de = {:.2e} c/w_pe", config::V_electrons);
  LOG_INFO("  cyclotron frequency,     Om_e = {:.2e} w_pe", config::Omega_max);
  LOG_INFO("  cyclotron period, 2 pi / Om_e = {:.2e} 1/w_pe ({} timesteps)",
    2 * M_PI / (config::Omega_max),
    int(2 * M_PI / (config::Omega_max) / dt));
  LOG_INFO("  cyclotron radius,       rho_e = {:.2e} c/w_pe ({} cells)",
    config::V_electrons / config::Omega_max,
    int(config::V_electrons / config::Omega_max / dx));
  LOG_FLUSH();

  LOG_TRACE("Initialization process...");
  std::list<Command_up> presets;

#if there_are_fields
  Fields_builder fields_builder;
  fields_ = Fields(fields_builder);

  // We use this to push the magnetic field to
  // the same half-timestep as electric field
  step_presets_.push_front(std::make_unique<Magnetic_field_half_step>(&fields_));

#if there_are_Bz0 && !START_FROM_BACKUP
  presets.push_back(std::make_unique<Set_Bz_distribution>(&fields_));
#endif
#endif

  Particles_builder particles_builder(fields_);

  Domain_geometry domain {
    config::domain_left,
    config::domain_right,
    config::domain_bottom,
    config::domain_top
  };


#if !BEAM_INJECTION_SETUP

#if !there_are_plasma_electrons
  particles_species_.reserve(2);
#else
  particles_species_.reserve(4);
#endif

  const int total_num_particles = int((__n.get_xmax() - config::domain_left) / dx) * SIZE_Y * config::Npi;
  LOG_INFO("{} particles will be set in total to each plasma specie", total_num_particles);

  particles_builder.set_sort("plasma_ions");
  Particles& plasma_ions = particles_species_.emplace_back(particles_builder);

  plasma_ions.particles_.reserve(2 * total_num_particles);

  plasma_ions.boundaries_processor_ = std::make_unique<Beam_boundary_processor>(
    plasma_ions.particles_, plasma_ions.parameters_, domain);

  presets.emplace_back(std::make_unique<Set_particles>(
    &plasma_ions, total_num_particles,
    Domain_geometry {
      floor(config::domain_left / dx),
      floor(__n.get_xmax() / dx),
      0,
      SIZE_Y
    },
    transition_layer::load_maxwellian_impulse
  ));


  Particles& buffer_ions = particles_species_.emplace_back(particles_builder);
  buffer_ions.sort_name_ = "buffer_plasma_ions";  // changed from "plasma_ions"

  buffer_ions.boundaries_processor_ = std::make_unique<Beam_buffer_processor>(
    buffer_ions.particles_, plasma_ions.particles_, plasma_ions.parameters_, domain);

  const int buffer_ions_size = config::BUFFER_SIZE * SIZE_Y * config::Npi;
  buffer_ions.particles_.reserve(2 * buffer_ions_size);

  step_presets_.emplace_back(std::make_unique<Set_particles>(
    &buffer_ions, buffer_ions_size,
    Domain_geometry {
      floor(config::domain_left / dx) - config::BUFFER_SIZE,
      floor(config::domain_left / dx),
      0,
      SIZE_Y
    },
    transition_layer::load_maxwellian_impulse
  ));

#if there_are_plasma_electrons
  particles_builder.set_sort("plasma_electrons");
  Particles& plasma_electrons = particles_species_.emplace_back(particles_builder);

  plasma_electrons.particles_.reserve(2 * total_num_particles);

  plasma_electrons.boundaries_processor_ = std::make_unique<Beam_boundary_processor>(
    plasma_electrons.particles_, plasma_electrons.parameters_, domain);

  presets.emplace_back(std::make_unique<Copy_coordinates>(
    &plasma_electrons, &plasma_ions, load_maxwellian_impulse
  ));


  Particles& buffer_electrons = particles_species_.emplace_back(particles_builder);
  buffer_electrons.sort_name_ = "buffer_plasma_electrons";  // changed from "plasma_electrons"

  buffer_electrons.particles_.reserve(2 * buffer_ions_size);

  buffer_electrons.boundaries_processor_ = std::make_unique<Beam_buffer_processor>(
    buffer_electrons.particles_, plasma_electrons.particles_, plasma_electrons.parameters_, domain);

  step_presets_.push_back(std::make_unique<Clone_layer_particles>(
    &plasma_electrons, &buffer_electrons, domain));

#endif

#else  // BEAM_INJECTION_SETUP

  particles_species_.reserve(2);

  particles_builder.set_sort("plasma_ions");
  Particles& plasma_ions = particles_species_.emplace_back(particles_builder);

  plasma_ions.boundaries_processor_ = std::make_unique<Beam_boundary_processor>(
    plasma_ions.particles_, plasma_ions.parameters_, domain);

  plasma_ions.particles_.reserve(config::PER_STEP_PARTICLES * TIME + 10'000);


  particles_builder.set_sort("plasma_electrons");
  Particles& plasma_electrons = particles_species_.emplace_back(particles_builder);

  plasma_electrons.boundaries_processor_ = std::make_unique<Beam_boundary_processor>(
    plasma_electrons.particles_, plasma_electrons.parameters_, domain);

  plasma_electrons.particles_.reserve(config::PER_STEP_PARTICLES * TIME + 10'000);


  step_presets_.emplace_back(std::make_unique<Ionize_particles>(
    &plasma_ions, &plasma_electrons,
    set_time_distribution(TIME, config::PER_STEP_PARTICLES * TIME),
    // set_point_of_birth:
    [](double *x, double *y) {
      *x = (0.5 * SIZE_X + (random_01() - 0.5) * config::WIDTH_OF_INJECTION_AREA) * dx;
      *y = random_01() * SIZE_Y * dy;
    },
    uniform_probability,
    load_maxwellian_impulse
  ));

#endif

  Diagnostics_builder diagnostics_builder(particles_species_, fields_);
  diagnostics_ = diagnostics_builder.build();

#if MAKE_BACKUPS || START_FROM_BACKUP
  auto backup =  std::make_unique<Simulation_backup>(
    /* backup timestep = */ 50,
    // named particle species to backup:
    std::unordered_map<std::string, Particles&>{
      { plasma_ions.sort_name_, plasma_ions },
      { plasma_electrons.sort_name_, plasma_electrons }
    },
    // named fields to backup:
    std::unordered_map<std::string, vector3_field&>{
      { "E", fields_.E() },
      { "B", fields_.B() }
  });

#if START_FROM_BACKUP
  START_ = backup->load();
#endif

#if MAKE_BACKUPS
  diagnostics_.emplace_back(std::move(backup));
#endif
#endif

#if !START_FROM_BACKUP
  for (const auto& command : presets) {
    command->execute(START_);
  }
#endif

  diagnose(START_);
  LOG_FLUSH();
}


void Manager::calculates() {
  for (size_t t = START_; t <= TIME; ++t) {
    LOG_TRACE("------------------------------------ one timestep ------------------------------------");
    PROFILE_SCOPE("one timestep");

    for (auto& command : step_presets_) {
      command->execute(t);
    }

#if there_are_particles
    for (auto& sort : particles_species_) {
      sort.push();
    }
#endif

    /// @todo Separate sources zeroing and field propagation
    /// @note Placed here to catch electric currents
    diagnose(t);

#if there_are_fields
    fields_.propagate();
#endif

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
