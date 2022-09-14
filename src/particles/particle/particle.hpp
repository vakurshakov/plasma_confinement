#ifndef SRC_PARTICLES_PARTICLE_PARTICLE_HPP 
#define SRC_PARTICLES_PARTICLE_PARTICLE_HPP

#include "src/particles/particle/point.hpp"
#include "src/particles/particle/parameters.hpp"

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
  const Parameters* parameters;  // pointer here to able to use move semantic 
};

#endif  //SRC_PARTICLES_PARTICLE_PARTICLE_HPP
