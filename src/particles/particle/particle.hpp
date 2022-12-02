#ifndef SRC_PARTICLES_PARTICLE_PARTICLE_HPP
#define SRC_PARTICLES_PARTICLE_PARTICLE_HPP

#include "src/particles/particle/point.hpp"
#include "src/particles/particle/parameters.hpp"

/**
 * @todo It would be easier to simplify the Particle
 * class to just a set of attributes without storing
 * pointers to outer parameters.
 */
class Particle {
 public:
  Particle(const Point& point, const Parameters& parameters)
    : point(point), parameters(&parameters) {}

  Point point;

#if IS_DENSITY_GLOBAL
  constexpr double n() const { return parameters->n(); }
#else
  double n_;
  constexpr double n() const { return n_; }
#endif

  constexpr double q() const { return parameters->q(); }
  constexpr double m() const { return parameters->m(); }

  vector3 velocity() const {
    return point.p / sqrt(m() * m() + point.p.square());
  }

 protected:
  /**
   * pointer here to be able to use move semantic.
   *
   * @warning
   * it will be ok if you move particle, but it
   * will cause a seg. fault after particles
   * being moved.
   */
  const Parameters* parameters;
};

#endif  //SRC_PARTICLES_PARTICLE_PARTICLE_HPP
