#ifndef SRC_SOLVERS_ABSTRACT_SOLVERS_HPP
#define SRC_SOLVERS_ABSTRACT_SOLVERS_HPP

#include "src/vectors/vector_classes.hpp"
#include "src/particles/particles.hpp"

namespace regular {

class Interpolation {
public:
  virtual ~Interpolation() = default;
  virtual void process(const vector2& r0,
    vector3& local_E, vector3& local_B) const = 0;
};


class Pusher {
public:
  virtual ~Pusher() = default;
  virtual void process(Particles::Particle_id,
    const vector3& local_E, const vector3& local_B) const = 0;
};


class Decomposition {
public:
  virtual ~Decomposition() = default;
  virtual void process(Particles::Particle_id, const vector2& r0) = 0;
};

}  // namespace regular

#endif  // SRC_SOLVERS_ABSTRACT_SOLVERS_HPP
