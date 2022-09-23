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
  Particle(size_t id, const Point& point, const Parameters& parameters)
    : point(point), id(id), parameters(&parameters) {}

  Point point;

  double n() const { return parameters->n(id); };
  double q() const { return parameters->q(id); };
  double m() const { return parameters->m(); }

 protected:
  size_t id;

  /**
   * pointer here to be able to use move semantic
   * @warn: it will be ok if you move particle, but
   * it will cause a seg. fault after particles
   * being moved.
   */
  const Parameters* parameters;
};

#endif  //SRC_PARTICLES_PARTICLE_PARTICLE_HPP
