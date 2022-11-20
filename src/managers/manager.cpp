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

#include "src/utils/random_number_generator.hpp"

void Manager::initializes() {
  PROFILE_FUNCTION();
  LOG_TRACE("Initialization process...");

  std::list<Command_up> presets;

#if there_are_fields
  Fields_builder fields_builder;
  fields_ = Fields(fields_builder);

  // We use this to push the magnetic field to
  // the same half-timestep as electric field
  step_presets_.push_front(std::make_unique<Magnetic_field_half_step>(&fields_));

#if there_are_Bz0
  presets.push_back(std::make_unique<Set_Bz_distribution>(&fields_));
#endif
#endif

#if BEAM_INJECTION_SETUP
  Particles_builder particles_builder(fields_);

  particles_species_.reserve(2);

  Domain_geometry domain{
    config::domain_left,
    config::domain_right,
    config::domain_bottom,
    config::domain_top
  };

  int total_num_particles = config::PER_STEP_PARTICLES * config::INJECTION_TIME + 10'000;

  particles_builder.set_sort("plasma_ions");
  Particles& plasma_ions = particles_species_.emplace_back(particles_builder);

  plasma_ions.boundaries_processor_ = std::make_unique<Beam_boundary_processor>(
    plasma_ions.particles_, plasma_ions.parameters_, domain);

  plasma_ions.particles_.reserve(total_num_particles);


  particles_builder.set_sort("plasma_electrons");
  Particles& plasma_electrons = particles_species_.emplace_back(particles_builder);

  plasma_electrons.boundaries_processor_ = std::make_unique<Beam_boundary_processor>(
    plasma_electrons.particles_, plasma_electrons.parameters_, domain);

  plasma_electrons.particles_.reserve(total_num_particles);

  step_presets_.emplace_back(std::make_unique<Ionize_particles>(
    &plasma_ions, &plasma_electrons,
    set_time_distribution(config::INJECTION_TIME, total_num_particles),
    transition_layer::set_on_segment,
    uniform_probability,
    load_maxwellian_impulse
  ));

#elif PLASMA_MAG_FIELD_SETUP
  Particles_builder particles_builder(fields_);

  particles_species_.reserve(
#if !there_are_plasma_electrons
  2
#else
  4
#endif
  );

  Domain_geometry domain{
    config::domain_left,
    config::domain_right,
    config::domain_bottom,
    config::domain_top
  };

  int total_num_particles = SIZE_X / 2 * SIZE_Y * config::Npi;
  LOG_INFO("{} particles will be set in total to each plasma specie", total_num_particles);

  particles_builder.set_sort("plasma_ions");
  Particles& plasma_ions = particles_species_.emplace_back(particles_builder);

  plasma_ions.boundaries_processor_ = std::make_unique<Plasma_boundary_processor>(
    plasma_ions.particles_, plasma_ions.parameters_, domain);

  struct Generate_on_half : public Coordinate_generator {
    void load(double* x, double* y) override {
      *x = config::domain_left + random_01() * (
          0.5 * SIZE_X * dx - config::domain_left);

      *y = random_01() * SIZE_Y * dy;
    }
  };

  presets.emplace_back(std::make_unique<Set_particles>(
    &plasma_ions, total_num_particles,
    std::make_unique<Generate_on_half>(),
    load_maxwellian_impulse
  ));


  Particles& buffer_ions = particles_species_.emplace_back(particles_builder);
  buffer_ions.sort_name_ = "buffer_plasma_ions";  // changed from "plasma_ions"

  int approximate_buffer_size = 5 * SIZE_Y * config::Npi + 100'000;
  buffer_ions.particles_.reserve(approximate_buffer_size);

  buffer_ions.boundaries_processor_ = std::make_unique<Plasma_buffer_processor>(
    buffer_ions.particles_, buffer_ions.parameters_);

  step_presets_.emplace_back(std::make_unique<Clone_layer_particles>(
    &plasma_ions, &buffer_ions, domain));


#if there_are_plasma_electrons
  particles_builder.set_sort("plasma_electrons");
  Particles& plasma_electrons = particles_species_.emplace_back(particles_builder);

  plasma_electrons.boundaries_processor_ = std::make_unique<Plasma_boundary_processor>(
    plasma_electrons.particles_, plasma_electrons.parameters_, domain);

  presets.emplace_back(std::make_unique<Copy_coordinates>(
    &plasma_electrons, &plasma_ions,
    load_maxwellian_impulse
  ));


  Particles& buffer_electrons = particles_species_.emplace_back(particles_builder);
  buffer_electrons.sort_name_ = "buffer_plasma_electrons";  // changed from "plasma_electrons"

  buffer_electrons.particles_.reserve(approximate_buffer_size);

  buffer_electrons.boundaries_processor_ = std::make_unique<Plasma_buffer_processor>(
    buffer_electrons.particles_, buffer_electrons.parameters_);

  step_presets_.emplace_back(std::make_unique<Clone_layer_particles>(
    &plasma_electrons, &buffer_electrons, domain));

#endif
#endif

  Diagnostics_builder diagnostics_builder(particles_species_, fields_);
  diagnostics_ = diagnostics_builder.build();

  for (const auto& command : presets) {
    command->execute(0);
  }

  diagnose(0);
}


void Manager::calculates() {
  for (size_t t = 1u; t <= TIME; ++t) {
    LOG_TRACE("------------------------------------ one timestep ------------------------------------");
    PROFILE_SCOPE("one timestep");

    for (const auto& command : step_presets_) {
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