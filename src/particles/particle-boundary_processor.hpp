#ifndef SRC_PARTICLES_PARTICLE_BOUNDARY_PROCESSOR_HPP
#define SRC_PARTICLES_PARTICLE_BOUNDARY_PROCESSOR_HPP

#include "src/pch.h"
#include "src/particles/particle/parameters.hpp"
#include "src/particles/particle/particle.hpp"
#include "src/particles/particle/point.hpp"
#include "src/vectors/vector_classes.hpp"

/**
 * @brief Small structure to represent the internal
 * domain, values should be passed in `c/wp`.
 */
struct Domain_geometry {
  double left;
  double right;
  double bottom;
  double top;

  Domain_geometry(double left, double right, double bottom, double top)
    : left(left), right(right), bottom(bottom), top(top) {}
};


class Particle_boundary_processor {
 public:
  Particle_boundary_processor(
    std::vector<Particle>& particles_vec, Parameters& params,
    Domain_geometry geom);

  virtual void add(Point& reference_point, const vector2& r0) = 0;
  virtual void remove() = 0;

 protected:
  std::vector<Particle>& particles_vec_;
  Parameters& params_;
  Domain_geometry geom_;

  vector3 generate_moment(const Point& reference_point);
};


class Plasma_boundary_processor : public Particle_boundary_processor {
 public:
  Plasma_boundary_processor(std::vector<Particle>& particles_vec,
    Parameters& params, Domain_geometry geom)
    : Particle_boundary_processor(particles_vec, params, geom) {}

  void add(Point& reference_point, const vector2& r0) override;
  void remove() override;

 private:
  inline bool passed_through_left(double x0, double ref_x) const;
  inline bool passed_through_right(double x0, double ref_x) const;
};


class Beam_boundary_processor : public Plasma_boundary_processor {
 public:
  Beam_boundary_processor(std::vector<Particle>& particles_vec,
    Parameters& params, Domain_geometry geom)
    : Plasma_boundary_processor(particles_vec, params, geom) {}

  void add(Point& reference_point, const vector2& r0) override {}
};

#endif  // SRC_PARTICLES_PARTICLE_BOUNDARY_PROCESSOR_HPP
