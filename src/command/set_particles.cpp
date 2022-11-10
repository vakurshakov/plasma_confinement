#include "set_particles.hpp"

#include "src/particles/particle/point.hpp"

Set_particles::Set_particles(
  Particles* const particles,
  std::size_t num_particles_to_load,
  std::unique_ptr<Coordinate_generator> coordinate_generator,
  std::function<void(double x, double y,
    double mass, double Tx, double Ty, double Tz,
    double p0, double* px, double* py, double* pz)> load_impulse)
  : particles_(particles),
    num_particles_to_load_(num_particles_to_load),
    coordinate_generator_(std::move(coordinate_generator)),
    load_impulse_(load_impulse) {}

void Set_particles::execute(int /* timestep */) const {
  PROFILE_FUNCTION();

  const double mass = particles_->get_parameters().m();
  const double Tx   = particles_->get_parameters().Tx();
  const double Ty   = particles_->get_parameters().Ty();
  const double Tz   = particles_->get_parameters().Tz();
  const double p0   = particles_->get_parameters().p0();

  // чтобы не происходило ресайзов неожиданных
  particles_->particles_.reserve(num_particles_to_load_ + 100'000);

  #pragma omp parallel for num_threads(NUM_THREADS)
  for (std::size_t p_id = 0u; p_id < num_particles_to_load_; ++p_id) {
    double x, y;
    coordinate_generator_->load(&x, &y);

    double px, py, pz;
    do {
      load_impulse_(x, y, mass, Tx, Ty, Tz, p0, &px, &py, &pz);
    }
    while (std::isinf(px) || std::isinf(py) || std::isinf(pz));

    particles_->add_particle({{x, y}, {px, py, pz}});
  }
}
