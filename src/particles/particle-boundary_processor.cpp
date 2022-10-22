#include "particle-boundary_processor.hpp"

#include <algorithm>

#include "src/utils/random_number_generator.hpp"

Particle_boundary_processor::Particle_boundary_processor(
    std::vector<Particle>& particles_vec, Parameters& params,
    Domain_geometry geom)
    : particles_vec_(particles_vec), params_(params),
      geom_(geom) {}

// This generate_moment method is problem-specific.
// As soon as our particles regenerated only near
// the left boundary, we give them a moment
// according to the initial distribution
vector3 Particle_boundary_processor::generate_moment(
    const Point& reference_point) {
  double new_py = params_.p0() * sin(2 * M_PI * random_01());
  double new_pz = 0.0;
  return { reference_point.px(), new_py, new_pz };
}


// If a particle moves from the zero cell to the first one,
// a new particle with the shifted x-coordinate is created
void Plasma_boundary_processor::add(
    Point& reference_point, const vector2& r0) {

  periodic_y(reference_point, geom_.bottom, geom_.top);

  vector2 new_r;
  bool particle_to_be_added = false;
  if (passed_through_left(r0.x(), reference_point.x())) {
    particle_to_be_added = true;
    new_r = { reference_point.x() - dx, reference_point.y() };
    LOG_TRACE("Particle passed through the left={:.5f}: x0={:.5f}, x={:.5f}",
      geom_.left, r0.x(), reference_point.x());
  }
  else if (passed_through_right(r0.x(), reference_point.x())) {
    particle_to_be_added = true;
    new_r = { reference_point.x() + dx, reference_point.y() };
    LOG_TRACE("Particle passed through the right={:.5f}: x0={:.5f}, x={:.5f}",
      geom_.right, r0.x(), reference_point.x());
  }

  if (!particle_to_be_added) return;
  LOG_TRACE("Creating a new one with x={:.5f}", new_r.x());

  vector3&& new_p = generate_moment(reference_point);

  /// @todo make emplace parallel with emplacing them by hand,
  /// maybe you should write a small and predictable container
  /// to simplify the process
  #pragma omp critical
  particles_vec_.emplace_back(particles_vec_.size(),
    Point{std::move(new_r), std::move(new_p)}, params_);
}

inline bool Plasma_boundary_processor::
passed_through_left(double x0, double ref_x) const {
  return
    x0 - geom_.left < dx &&
    ref_x - geom_.left > dx;
}

inline bool Plasma_boundary_processor::
passed_through_right(double x0, double ref_x) const {
  return
    geom_.right - x0 < dx &&
    geom_.right - ref_x > dx;
}

// Passing of a particle to the buffer cell is
// followed by its removing from the simulation
void Plasma_boundary_processor::remove() {
  PROFILE_FUNCTION();

  auto new_last = std::remove_if(
    particles_vec_.begin(),
    particles_vec_.end(),
    [&] (const Particle& particle) {
      return
        particle.point.x() < geom_.left ||
        particle.point.x() > geom_.right;
  });

  if (new_last != particles_vec_.end()) {
    LOG_TRACE("{} particle(s) removed", particles_vec_.end() - new_last);
    particles_vec_.erase(new_last, particles_vec_.end());
  }
}
