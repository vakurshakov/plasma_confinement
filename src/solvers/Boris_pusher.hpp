#ifndef SRC_SOLVERS_BORIS_PUSHER_HPP
#define SRC_SOLVERS_BORIS_PUSHER_HPP

#include "abstract_solvers.hpp"

#include "src/pch.h"
#include "src/particles/concrete/particles_2d3v.hpp"
#include "src/vectors/vector_classes.hpp"

namespace regular {

class Boris_pusher : public Pusher {
 public:
  Boris_pusher(Particles2D3V& particles);

  void process(Particles::Particle_id,
    const vector3& local_E, const vector3& local_B) const override;

 private:
  Particles2D3V& particles;
};

}  // namespace regular

#endif  // SRC_SOLVERS_BORIS_PUSHER_HPP
