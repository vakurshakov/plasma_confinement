#include "set_particles.hpp"

#include "src/particles/particle/point.hpp"
#include "src/utils/random_number_generator.hpp"

Set_particles::Set_particles(
  Particles* const particles,
  const Domain_geometry& circumscribing_rectangle,
  const Filling_condition& filling_condition,
  const Density_profile& density_profile,
  const Coordinate_generator& coordinate_generator,
  const Momentum_generator& momentum_generator)
  : particles_(particles),
    circumscribing_rectangle_(circumscribing_rectangle),
    cell_should_be_filled_(filling_condition),
    get_density_profile_(density_profile),
    load_coordinate_(coordinate_generator),
    load_momentum_(momentum_generator) {}


void Set_particles::execute(int /* timestep */) {
  PROFILE_FUNCTION();
  LOG_TRACE("Setting {} distribution", particles_->get_parameters().get_name());

  static const int Np = particles_->get_parameters().Np();

  static const auto& geom = circumscribing_rectangle_;
  static const int nx_min = static_cast<int>(floor(geom.x_min / dx));
  static const int nx_max = static_cast<int>(floor(geom.x_max / dx));
  static const int ny_min = static_cast<int>(floor(geom.y_min / dy));
  static const int ny_max = static_cast<int>(floor(geom.y_max / dy));

  // to prevent unintended resizes, thus execute() should be called once
  particles_->particles_.reserve((nx_max - nx_min) * (ny_max - ny_min) + 10'000);

  /// @todo iterators can be used to provide custom cell-traversing
  #pragma omp parallel for num_threads(OMP_NUM_THREADS)
  for (int ny = ny_min; ny < ny_max; ++ny) {
  for (int nx = nx_min; nx < nx_max; ++nx) {
    if (!cell_should_be_filled_(nx, ny))
      continue;

#if GLOBAL_DENSITY
    int particles_in_cell = static_cast<int>(
      floor(Np * get_density_profile_(nx, ny)));

    if (particles_in_cell <= 0)
      continue;

    for (int p = 0; p < particles_in_cell; ++p) {
      vector2 coordinate = load_coordinate_(nx, ny);
      vector3 momentum = load_momentum_(coordinate);

      particles_->add_particle(
        Point{std::move(coordinate), std::move(momentum)});
    }
#else
    for (int p = 0; p < Np; ++p) {
      vector2 coordinate = load_coordinate(nx, ny);
      vector3 momentum = load_momentum(coordinate);

      particles_->add_particle(
        Point{std::move(coordinate), std::move(momentum)},
        get_density_profile(nx, ny));
    }
#endif
  }}
}
