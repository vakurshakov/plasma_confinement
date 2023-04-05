#include "set_particles.hpp"

#include "src/particles/particle/point.hpp"
#include "src/utils/random_number_generator.hpp"

Set_particles::Set_particles(
  Particles* const particles,
  std::size_t num_particles_to_load,
  std::unique_ptr<Coordinate_generator> coordinate_generator,
  const impulse_loader& load_impulse)
  : particles_(particles),
    num_particles_to_load_(num_particles_to_load),
    coordinate_generator_(std::move(coordinate_generator)),
    load_impulse_(load_impulse) {}


Set_particles::Set_particles(
  Particles* const particles,
  std::size_t num_particles_to_load,
  const Domain_geometry& geom,
  const impulse_loader& load_impulse)
  : particles_(particles),
    num_particles_to_load_(num_particles_to_load),
    geom_(geom),
    load_impulse_(load_impulse) {}


void Set_particles::execute(int /* timestep */) {
  PROFILE_FUNCTION();
  LOG_TRACE("Setting {} distribution", particles_->get_parameters().get_name());

  const double mass = particles_->get_parameters().m();
  const double Tx   = particles_->get_parameters().Tx();
  const double Ty   = particles_->get_parameters().Ty();
  const double Tz   = particles_->get_parameters().Tz();
  const double p0   = particles_->get_parameters().p0();

  // чтобы не происходило ресайзов неожиданных
  // particles_->particles_.reserve(num_particles_to_load_ + 100'000);

  /// @todo Think carefully to combine two ways of setting particles:
  /// 1. On the one hand it is hard to load particles randomly in parallel
  /// 2. It is not always possible to describe everything in such a cycle.

  for (int i = int(geom_.x_min); i < int(geom_.x_max); ++i) {

  #pragma omp parallel for num_threads(NUM_THREADS)
  for (int j = int(geom_.y_min); j < int(geom_.y_max); ++j) {
    for (int np = 0; np < config::Npi; ++np) {
      double x = (i + random_01()) * dx;
      double y = (j + random_01()) * dy;

      double px, py, pz;
      do {
        load_impulse_(x, y, mass, Tx, Ty, Tz, p0, &px, &py, &pz);
      }
      while (std::isinf(px) || std::isinf(py) || std::isinf(pz));

      particles_->add_particle({{x, y}, {px, py, pz}});
  }}}
}
