#include "manager.hpp"

#include "src/command/set_Bz_distribution.hpp"
#include "src/command/magnetic_field_half_step.hpp"
#include "src/command/set_particles.hpp"
#include "src/command/clone_layer_particles.hpp"

#include "src/fields/fields_builder.hpp"
#include "src/particles/particles_builder.hpp"
#include "src/diagnostics/diagnostics_builder.hpp"

#include "src/utils/transition_layer/particles_distribution.hpp"

void Manager::initializes() {
  PROFILE_FUNCTION();
  LOG_TRACE("Initialization process...");

  std::list<Command_up> presets;

#if there_are_fields
  Fields_builder fields_builder;
  fields_ = Fields(fields_builder);

  step_presets_.push_front(std::make_unique<Magnetic_field_half_step>(&fields_));

#if there_are_Bz0
  presets.push_back(std::make_unique<Set_Bz_distribution>(&fields_));
#endif
#endif

#if there_are_particles && there_are_plasma_ions
  Particles_builder particles_builder(fields_);

  particles_builder.set_sort("plasma_ions");
  Particles& plasma_ions = particles_species_.emplace_back(particles_builder);

  auto generator = std::make_unique<transition_layer::Random_coordinate_generator>();
  int num_particles_to_load = generator->get_particles_number();

  LOG_INFO("{} plasma ions will be set", num_particles_to_load);

  presets.push_back(std::make_unique<Set_particles>(
    &plasma_ions, num_particles_to_load,
    std::move(generator),
    transition_layer::load_ions_impulse
  ));

  step_presets_.push_back(std::make_unique<Clone_layer_particles>(
    &plasma_ions,
    Domain_geometry(
      config::domain_left,
      config::domain_right,
      config::domain_bottom,
      config::domain_top
    )
  ));
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
    PROFILE_SCOPE("one timestep");

#if there_are_particles
    for (auto& sort : particles_species_) {
      sort.push();
    }
#endif

#if there_are_fields
    fields_.propagate();
#endif

    diagnose(t);
  }
}

void Manager::diagnose(size_t t) const {
  PROFILE_FUNCTION();

  #pragma omp parallel for shared(diagnostics_), num_threads(NUM_THREADS)
  for (const auto& diagnostic : diagnostics_) {
      diagnostic->diagnose(t);
  }
}