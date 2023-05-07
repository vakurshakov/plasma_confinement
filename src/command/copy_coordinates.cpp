#include "copy_coordinates.hpp"

Copy_coordinates::Copy_coordinates(
    Particles* const particles_copy_to,
    const Particles* particles_copy_from,
    const Momentum_generator& load_momentum)
    : particles_copy_to_(particles_copy_to),
      particles_copy_from_(particles_copy_from),
      load_momentum_(load_momentum) {}

void Copy_coordinates::execute(int /* timestep */) {
  LOG_TRACE("Setting distribution for {}, copying coordinates from {}",
    particles_copy_from_->get_parameters().get_name(),
    particles_copy_to_->get_parameters().get_name());

  particles_copy_to_->particles_.reserve(particles_copy_from_->particles_.capacity());

  for (const auto& particle : particles_copy_from_->particles_) {
    vector3 momentum = load_momentum_(particle.point.r);

#if GLOBAL_DENSITY
    particles_copy_to_->add_particle(Point{particle.point.r, std::move(momentum)});
#else
    particles_copy_to_->add_particle(Point{particle.point.r, std::move(momentum)}, particle.n());
#endif
  }
}
