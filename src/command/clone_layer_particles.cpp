#include "clone_layer_particles.hpp"

Clone_layer_particles::Clone_layer_particles(
  Particles* const particles_inout,
  Domain_geometry geom)
  : particles_in_(particles_inout),
    particles_out_(particles_inout),
    geom_(geom) {}

Clone_layer_particles::Clone_layer_particles(
  Particles* const particles_in,
  Particles* const particles_out,
  Domain_geometry geom)
  : particles_in_(particles_in),
    particles_out_(particles_out),
    geom_(geom) {}

void Clone_layer_particles::execute(int /* timestep */) {
  PROFILE_FUNCTION();

  auto particles_fixed_end = particles_in_->particles_.end();

  #pragma omp parallel for num_threads(NUM_THREADS)
  for (auto it = particles_in_->particles_.begin(); it != particles_fixed_end; ++it) {
    if (particle_should_be_cloned(it->point)) {
#if GLOBAL_DENSITY
      particles_out_->add_particle(configure_point(it->point));
#else
      particles_out_->add_particle(configure_point(it->point), it->n());
#endif
    }
  }
}

inline bool
Clone_layer_particles::particle_should_be_cloned(const Point& point) const {
  double width = config::BUFFER_SIZE * dx;

  return particle_on_the_left(point.x(), width) ||
    particle_on_the_right(point.x(), width);
}

inline bool
Clone_layer_particles::particle_on_the_left(double x, double width) const {
  return x > geom_.left && x < geom_.left + width;
}

inline bool
Clone_layer_particles::particle_on_the_right(double x, double width) const {
  return x > geom_.right - width && x < geom_.right;
}

inline Point
Clone_layer_particles::configure_point(const Point& point) const {
  double width = config::BUFFER_SIZE * dx;

  double new_x = particle_on_the_left(point.x(), width)?
    2 * geom_.left - point.x():
    2 * geom_.right - point.x();

  return Point{{new_x, point.y()}, point.p};
}
