#include "copy_coordinates.hpp"

Copy_coordinates::Copy_coordinates(
    Particles* const particles_copy_to,
    const Particles* const particles_copy_from,
    const impulse_loader& load_impulse)
    : particles_copy_to_(particles_copy_to),
      particles_copy_from_(particles_copy_from),
      load_impulse_(load_impulse) {}

void Copy_coordinates::execute(int /* timestep */) {
  LOG_TRACE("Setting distribution for {}, copying coordinates from {}",
    particles_copy_from_->get_parameters().get_name(),
    particles_copy_to_->get_parameters().get_name());

  const double mass = particles_copy_to_->get_parameters().m();
  const double Tx   = particles_copy_to_->get_parameters().Tx();
  const double Ty   = particles_copy_to_->get_parameters().Ty();
  const double Tz   = particles_copy_to_->get_parameters().Tz();
  const double p0   = particles_copy_to_->get_parameters().p0();

  // particles_copy_to_->particles_.reserve(particles_copy_from_->particles_.capacity());

  for (const auto& particle : particles_copy_from_->particles_) {
    double x = particle.point.x();
    double y = particle.point.y();

    double px, py, pz;
    do {
      load_impulse_(x, y, mass, Tx, Ty, Tz, p0, &px, &py, &pz);
    }
    while (std::isinf(px) || std::isinf(py) || std::isinf(pz));

#if GLOBAL_DENSITY
    particles_copy_to_->add_particle({{x, y}, {px, py, pz}});
#else
    particles_copy_to_->add_particle({{x, y}, {px, py, pz}}, particle.n());
#endif
  }
}
