#include "manager.hpp"

#include "src/command/cmd_add_Bz0.hpp"
#include "src/command/magnetic_field_half_step.hpp"
#include "src/command/set_particles.hpp"
#include "src/command/clone_layer_particles.hpp"

#include "src/fields/fields_builder.hpp"
#include "src/particles/particles_builder.hpp"
#include "src/diagnostics/diagnostics_builder.hpp"

void Manager::initializes() {
  PROFILE_FUNCTION();

  std::list<Command_up> presets;

#if there_are_fields
  Fields_builder fields_builder;
  fields_ = Fields(fields_builder);

  step_presets_.push_front(std::make_unique<Magnetic_field_half_step>(&fields_));

#if there_are_Bz0
  presets.push_back(std::make_unique<Add_Bz0>(&fields_, config::Bz0));
#endif
#endif

  Particles_builder particles_builder(fields_);

  particles_builder.set_sort("plasma_electrons");
  Particles& plasma_electrons = particles_species_.emplace_back(particles_builder);

  presets.push_back(std::make_unique<Set_particles>(
    &plasma_electrons, 10'000 /* чтобы не происходило ресайзов неожиданных */,
    [] (int cx, int cy) -> bool {
      return
        (cx == config::domain_left / dx + 3 && cy == SIZE_Y / 2 + 5) ||
        (cx == config::domain_left / dx + 3 && cy == SIZE_Y / 2 - 5);
    },
    [] (double x, double y,
        double mass, double Tx, double Ty, double Tz,
        double p0, double *px, double *py, double *pz) -> void {
      *px = -1e-2;
      *py = 0.0;
      *pz = 0.0;
    },
    [] (int, int, int nx, int ny, double *x, double *y) -> void {
      *x = nx * dx;
      *y = ny * dy;
    }
  ));

  step_presets_.push_back(std::make_unique<Clone_layer_particles>(
    &plasma_electrons,
    Domain_geometry(
      config::domain_left,
      config::domain_right,
      config::domain_bottom,
      config::domain_top
    )
  ));

  Diagnostics_builder diagnostics_builder(particles_species_, fields_);
  diagnostics_ = diagnostics_builder.build();

  for (const auto& command : presets) {
    command->execute(0);
  }

  diagnose(0);
}

void Manager::calculates() {
  for (int t = 1; t <= TIME; ++t) {
    PROFILE_SCOPE("one timestep");

    // Removing unnecessary elements.
    step_presets_.remove_if(
    [t] (const Command_up& command) {
      return command->needs_to_be_removed(t);
    });

    // Executing the rest of the commands.
    for (const auto& command : step_presets_) {
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
  }
}

void Manager::diagnose(int t) const {
  PROFILE_FUNCTION();

  #pragma omp parallel for shared(diagnostics_), num_threads(NUM_THREADS)
  for (const auto& diagnostic : diagnostics_) {
      diagnostic->diagnose(t);
  }
}