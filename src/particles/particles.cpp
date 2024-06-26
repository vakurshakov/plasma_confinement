#include "particles.hpp"

#include <algorithm>

#include "src/vectors/vector_classes.hpp"
#include "src/particles/particles_load.hpp"

Particles::Particles(Particles_builder& builder) {
  sort_name_ = builder.get_sort_name();
  parameters_ = builder.build_parameters();

  push_ = builder.build_pusher();
  interpolation_ = builder.build_interpolation(parameters_);
  decomposition_ = builder.build_decomposition(parameters_);
}

void Particles::push() {
  PROFILE_FUNCTION();

  /**
   * We fix the end iterator in case of adding particles.
   *
   * @warning Capacity should be set in advance. Algorithm
   * ends up with segfault if reallocation of particles
   * array happens.
   */
  auto particles_fixed_end = particles_.end();

#if TIME_PROFILING
  int size = particles_fixed_end - particles_.begin();
#endif

  #pragma omp for schedule(monotonic: dynamic, CHUNK_SIZE)
  for (auto it = particles_.begin(); it != particles_fixed_end; ++it) {
    vector2 r0 = it->point.r;
    vector3 local_E = {0.0, 0.0, 0.0};
    vector3 local_B = {0.0, 0.0, 0.0};

    ACCUMULATIVE_PROFILE("field interpolation process", size,
      interpolation_->process(r0, local_E, local_B));

    ACCUMULATIVE_PROFILE("particle push process", size,
      push_->process(*it, local_E, local_B));

    ACCUMULATIVE_PROFILE("current decomposition process", size,
      decomposition_->process(*it, r0));

    ACCUMULATIVE_PROFILE("adding particles via open boundaries", size,
      boundaries_processor_->add(*it, r0));
  }

  /// @todo we can put here nowait clause, but we than
  /// have to carefully trace particles_fixed_end.
  #pragma omp single
  {
    boundaries_processor_->remove();
    LOG_WARN("Number of {} after `void Particles::push()`: {}",  sort_name_, particles_.size());
  }
}

#if GLOBAL_DENSITY
void Particles::add_particle(const Point& point) {
  #pragma omp critical
  particles_.emplace_back(point, parameters_);
}

#else
void Particles::add_particle(const Point& point, double local_n) {
  #pragma omp critical
  {
    Particle& new_particle = particles_.emplace_back(point, parameters_);
    new_particle.n_ = local_n;
  }
}

#endif
