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
  assert(config::V_ions < 1 && "ions velocity is greater the c!");

  Particles_builder particles_builder(fields_);

  particles_species_.reserve(2);

  particles_builder.set_sort("plasma_ions");
  Particles& plasma_ions = particles_species_.emplace_back(particles_builder);

  plasma_ions.boundaries_processor_ = std::make_unique<Plasma_boundary_processor>(
    plasma_ions.particles_, plasma_ions.parameters_,
    Domain_geometry(
      config::domain_left,
      config::domain_right,
      config::domain_bottom,
      config::domain_top
    )
  );

  auto generator = std::make_unique<transition_layer::Random_coordinate_generator>();
  int num_particles_to_load = generator->get_particles_number();

  LOG_INFO("{} plasma ions will be set", num_particles_to_load);

  presets.push_back(std::make_unique<Set_particles>(
    &plasma_ions, num_particles_to_load,
    std::move(generator),
    transition_layer::load_ions_impulse
  ));

  Particles& buffer_ions = particles_species_.emplace_back(particles_builder);
  buffer_ions.sort_name_ = "buffer_plasma_ions";  // changed from "plasma_ions"

  buffer_ions.boundaries_processor_ = std::make_unique<Buffer_processor>(
    buffer_ions.particles_, buffer_ions.parameters_
  );

  step_presets_.push_back(std::make_unique<Clone_layer_particles>(
    &plasma_ions,
    &buffer_ions,
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
    LOG_TRACE("------------------------------ one timestep ------------------------------");
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