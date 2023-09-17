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
}


void Manager::initializes() {
  PROFILE_FUNCTION();

  Manager::log_information();
  LOG_TRACE("Initialization process...");

  std::list<Command_up> presets;

  Fields_builder fields_builder;
  fields_ = Fields(fields_builder);

#if there_are_fields
  // We use this to push the magnetic field to
  // the same half-timestep as electric field
  step_presets_.push_front(std::make_unique<Magnetic_field_half_step>(&fields_));

#if there_are_Bz0 && !START_FROM_BACKUP
  presets.push_back(std::make_unique<Set_Bz_distribution>(&fields_));
#endif
#endif

#if START_FROM_BACKUP
  Simulation_backup::restore_time_diagnostics();
#endif

  Particles_builder particles_builder(fields_);

  Domain_geometry domain {
    config::domain_r_min,
    config::domain_r_max,
    config::domain_phi_min,
    config::domain_phi_max
  };

#if there_are_target_plasma
  particles_species_.reserve(4);
#else
  particles_species_.reserve(2);
#endif

  particles_builder.set_sort("plasma_ions");
  Particles& plasma_ions = particles_species_.emplace_back(particles_builder);

  plasma_ions.boundaries_processor_ = std::make_unique<POL_Beam_boundary>(
    plasma_ions.particles_, plasma_ions.parameters_, domain);

  plasma_ions.particles_.reserve(config::PER_STEP_PARTICLES * TIME + 10'000);


  particles_builder.set_sort("plasma_electrons");
  Particles& plasma_electrons = particles_species_.emplace_back(particles_builder);

  plasma_electrons.boundaries_processor_ = std::make_unique<POL_Beam_boundary>(
    plasma_electrons.particles_, plasma_electrons.parameters_, domain);

  plasma_electrons.particles_.reserve(config::PER_STEP_PARTICLES * TIME + 10'000);

  step_presets_.emplace_back(std::make_unique<Ionize_particles>(
    &plasma_ions, &plasma_electrons,
    set_time_distribution(TIME, config::PER_STEP_PARTICLES * TIME),
    // set_point_on_circle:
    [](double* x, double* y) {
      static const double center_x = 0.5 * SIZE_X * dx;
      static const double center_y = 0.5 * SIZE_Y * dy;
      static const double r0 = config::R0;

      double r = r0 * sqrt(random_01());
      double phi = 2.0 * M_PI * random_01();

      *x = center_x + r * cos(phi);
      *y = center_y + r * sin(phi);
    },
    uniform_probability,
    load_maxwellian_impulse
  ));


#if there_are_target_plasma
  particles_builder.set_sort("target_ions");
  Particles& target_ions = particles_species_.emplace_back(particles_builder);

  target_ions.boundaries_processor_ = std::make_unique<POL_Beam_boundary>(
    target_ions.particles_, target_ions.parameters_, domain);

  const int total_number_of_ions =
    M_PI * config::RADIUS_OF_TARGET_PLASMA * config::RADIUS_OF_TARGET_PLASMA * config::Npi / (dx * dy);

  struct Set_coordinate_on_circle : public Coordinate_generator {
    const double center_x = 0.5 * SIZE_X * dx;
    const double center_y = 0.5 * SIZE_Y * dy;
    const double R_max = config::RADIUS_OF_TARGET_PLASMA;

    Set_coordinate_on_circle() = default;

    void load(double* x, double* y) override {
      double r = R_max * sqrt(random_01());
      double phi = 2.0 * M_PI * random_01();

      *x = center_x + r * cos(phi);
      *y = center_y + r * sin(phi);
    }
  };

  presets.emplace_back(std::make_unique<Set_particles>(
    &target_ions, total_number_of_ions,
    std::make_unique<Set_coordinate_on_circle>(),
    load_maxwellian_impulse));


  particles_builder.set_sort("target_electrons");
  Particles& target_electrons = particles_species_.emplace_back(particles_builder);

  target_electrons.boundaries_processor_ = std::make_unique<POL_Beam_boundary>(
    target_electrons.particles_, target_electrons.parameters_, domain);

  presets.emplace_back(std::make_unique<Copy_coordinates>(
    &target_electrons, &target_ions, load_maxwellian_impulse));

#endif

#if MAKE_BACKUPS || START_FROM_BACKUP
  auto backup =  std::make_unique<Simulation_backup>(
    /* backup timestep = */ backup_time_step,
    // named particle species to backup:
    std::unordered_map<std::string, Particles&>{
      { plasma_ions.sort_name_, plasma_ions },
      { plasma_electrons.sort_name_, plasma_electrons },
#if there_are_target_plasma
      { target_ions.sort_name_, target_ions },
      { target_electrons.sort_name_, target_electrons },
#endif
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

  /**
   * @warning This parallelization produces the following problem.
   * Enabling it with no if condition, reduces the cost of running
   * _all_ diagnostics (i.e. when t % diagnose_time_step == 0)
   * by ~3.7, but on other timesteps the cost of thread creation
   * accumulates and drops the overall performance by ~1.5.
   */
  #pragma omp parallel for shared(diagnostics_), if(t % diagnose_time_step == 0)
  for (const auto& diagnostic : diagnostics_) {
    diagnostic->diagnose(t);
  }
}
