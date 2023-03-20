#include "particle-boundary_processor.hpp"

#include <algorithm>

#include "src/utils/random_number_generator.hpp"
#include "src/particles/particles_load.hpp"

Particle_boundary_processor::Particle_boundary_processor(
    std::vector<Particle>& particles_vec,
    Parameters& params, Domain_geometry geom)
    : particles_vec_(particles_vec), params_(params), geom_(geom) {}

vector3 Particle_boundary_processor::
generate_moment(const Particle& particle) {
  double new_px = random_sign() * particle.point.px();
  double new_py = random_sign() * particle.point.py();

#if _2D3V
  double new_pz = random_sign() * particle.point.pz();
#else
  double new_pz = 0.0;
#endif

  return { new_px, new_py, new_pz };
}


Plasma_boundary_processor::Plasma_boundary_processor(
    std::vector<Particle>& particles_vec,
    Parameters& params, Domain_geometry geom)
    : Particle_boundary_processor(particles_vec, params, geom) {}

// If a particle moves from the zero cell to the first one,
// a new particle with the shifted x-coordinate is created
void Plasma_boundary_processor::
add(Particle& particle, const vector2& r0) {

  periodic_y(particle.point, geom_.bottom, geom_.top);

  if (passed_through_left(r0.x(), particle.point.x())) {
    vector2&& new_r = { particle.point.x() - dx, particle.point.y() };
    vector3&& new_p = generate_moment(particle);

    /// @todo make emplace parallel with emplacing them by hand,
    /// maybe you should write a small and predictable container
    /// to simplify the process
#if GLOBAL_DENSITY
    #pragma omp critical
    particles_vec_.emplace_back(Point{std::move(new_r), std::move(new_p)}, params_);

#else
    #pragma omp critical
    {
      Particle& new_particle = particles_vec_.emplace_back(
        Point{ std::move(new_r), std::move(new_p) }, params_);

      new_particle.n_ = particle.n_;
    }

#endif
  }
}

inline bool Plasma_boundary_processor::
passed_through_left(double x0, double ref_x) const {
  return
    x0    - (geom_.left + dx) < 0 &&
    ref_x - (geom_.left + dx) > 0;
}

inline bool Plasma_boundary_processor::
passed_through_right(double x0, double ref_x) const {
  return
    x0    - (geom_.right - dx) > 0 &&
    ref_x - (geom_.right - dx) < 0;
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
        particle.point.x() <= geom_.left ||
        particle.point.x() >= geom_.right;
  });

  if (new_last != particles_vec_.end()) {
    LOG_TRACE("{} particle(s) removed", particles_vec_.end() - new_last);
    particles_vec_.erase(new_last, particles_vec_.end());
  }
}


Reflective_boundary_processor::Reflective_boundary_processor(
    std::vector<Particle>& particles_vec,
    Parameters& params, Domain_geometry geom)
    : Plasma_boundary_processor(particles_vec, params, geom) {}

void Reflective_boundary_processor::
add(Particle& particle, const vector2& r0) {

  periodic_y(particle.point, geom_.bottom, geom_.top);

  if (particle.point.x() < geom_.left) {
    particle.point.x() += dx;
    particle.point.p = generate_moment(particle);
  }
}

void Reflective_boundary_processor::remove() {
  PROFILE_FUNCTION();

  auto new_last = std::remove_if(
    particles_vec_.begin(),
    particles_vec_.end(),
    [&] (const Particle& particle) {
      return
        particle.point.x() > geom_.right;
  });

  if (new_last != particles_vec_.end()) {
    LOG_TRACE("{} particle(s) removed", particles_vec_.end() - new_last);
    particles_vec_.erase(new_last, particles_vec_.end());
  }
}

Beam_boundary_processor::Beam_boundary_processor(
    std::vector<Particle>& particles_vec,
    Parameters& params, Domain_geometry geom)
    : Plasma_boundary_processor(particles_vec, params, geom) {}

void Beam_boundary_processor::
add(Particle& particle, const vector2& r0) {
  periodic_y(particle.point, geom_.bottom, geom_.top);
}


Beam_buffer_processor::Beam_buffer_processor(
    std::vector<Particle>& buff_beam_vec,
    std::vector<Particle>& main_beam_vec,
    Parameters& main_params,
    Domain_geometry geom)
    : Plasma_boundary_processor(buff_beam_vec, main_params, geom),
      main_beam_vec_(main_beam_vec) {}

void Beam_buffer_processor::
add(Particle& particle, const vector2& r0) {
  periodic_y(particle.point, geom_.bottom, geom_.top);
}

void Beam_buffer_processor::remove() {
  PROFILE_FUNCTION();

  // Firstly, we remove particle that wasn't pass the border
  Plasma_boundary_processor::remove();

  // Secondly, we move remaining particles to the main vector
  LOG_TRACE("{} particles will come from buffer", particles_vec_.size());

  for (const auto& particle : particles_vec_) {
#if GLOBAL_DENSITY
    main_beam_vec_.emplace_back(
      Point {
        { particle.point.x(),  particle.point.y() },
        { particle.point.px(), particle.point.py(), particle.point.pz() }
      },
      params_);
#else
    Particle& new_one = main_beam_vec_.emplace_back(
      Point {
        { particle.point.x(),  particle.point.y() },
        { particle.point.px(), particle.point.py(), particle.point.pz() }
      },
      params_);
    new_one.n_ = particle.n_;
#endif
  }

  LOG_TRACE("{} particles in the {} after merging buffer",
    main_beam_vec_.size(), params_.sort_name_);

  // Thirdly, clear the buffer
  particles_vec_.clear();
}
