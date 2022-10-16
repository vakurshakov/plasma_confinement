#include "particles.hpp"

#include <cstdarg>

#include "src/vectors/vector_classes.hpp"
#include "src/particles/particles_load.hpp"

Particles::Particles(Particles_builder& builder) {
  sort_name_ = builder.get_sort_name();
  parameters_ = builder.build_parameters();

  push_ = builder.build_pusher();
  interpolation_ = builder.build_interpolation(this->parameters_);
  decomposition_ = builder.build_decomposition(this->parameters_);

  boundaries_processor_ = std::make_unique<Plasma_boundary_processor>(
    this->particles_, this->parameters_,
    Domain_geometry(
      config::domain_left,
      config::domain_right,
      config::domain_bottom,
      config::domain_top
    )
  );
}

/// @warning Algorithm ends up with seg. fault
/// if reallocation of particles array happens
void Particles::push() {
  PROFILE_FUNCTION();

  auto push = std::mem_fn(&Pusher::process);
  auto decompose = std::mem_fn(&Decomposition::process);
  auto interpolate = std::mem_fn(&Interpolation::process);

  auto particles_fixed_end = this->particles_.end();
  int size = particles_fixed_end - particles_.begin();

// firstprivate attribute initializes the thread-local variables
// with the values of the original object outside of this scope
#pragma omp parallel num_threads(NUM_THREADS),\
  firstprivate(push, interpolate, decompose)
{
  #pragma omp for
  for (auto it = particles_.begin(); it != particles_fixed_end; ++it) {
    vector2 r0 = it->point.r;
    vector3 local_E = {0., 0., 0.};
    vector3 local_B = {0., 0., 0.};

    ACCUMULATIVE_PROFILE("field interpolation process", size,
      interpolate(interpolation_, r0, local_E, local_B));

    ACCUMULATIVE_PROFILE("particle push process", size,
      push(push_, *it, local_E, local_B));

    ACCUMULATIVE_PROFILE("current decomposition process", size,
      decompose(decomposition_, *it, r0));

    ACCUMULATIVE_PROFILE("adding particles via open boundaries", size,
      boundaries_processor_->add(it->point, r0));
  }
}
  boundaries_processor_->remove();
}

/**
 * @brief Adds a particle to the end of the array.
 *
 * @param point Coordinates and impulse of the particle.
 * @param ... Local parameters.
 */
void Particles::add_particle(const Point& point, ...) {
  va_list list;
  va_start(list, point);

  if (parameters_.n_type() == "local")
    parameters_.set_n(static_cast<double>(va_arg(list, double)));

  if (parameters_.q_type() == "local")
    parameters_.set_q(static_cast<double>(va_arg(list, double)));

  va_end(list);

  #pragma omp critical
  particles_.emplace_back(particles_.size(), point, parameters_);
}
