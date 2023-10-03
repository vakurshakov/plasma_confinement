#include "clone_layer_particles.hpp"

#include "src/utils/random_number_generator.hpp"


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

  #pragma omp parallel for
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
  return particle_on_the_left(point.x());
}

inline bool
Clone_layer_particles::particle_on_the_left(double x) const {
  return x > geom_.x_min && x < geom_.x_min + width_;
}

inline bool
Clone_layer_particles::particle_on_the_right(double x) const {
  return x > geom_.x_max - width_ && x < geom_.x_max;
}

inline Point
Clone_layer_particles::configure_point(const Point& point) const {
  return Point{
    { geom_.x_min - random_01() * width_, random_01() * SIZE_Y * dx },
    { random_sign() * point.px(), random_sign() * point.py(), 0.0 },
  };
}
