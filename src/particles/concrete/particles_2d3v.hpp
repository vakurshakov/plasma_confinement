#ifndef SRC_PARTICLES_CONCRETE_PARTICLES_2D3V_HPP
#define SRC_PARTICLES_CONCRETE_PARTICLES_2D3V_HPP

#include "src/particles/particles.hpp"
#include "src/particles/sort_components.hpp"

namespace regular {

struct Particles2D3V : public Particles {
  Coordinate2D coordinate;
  Momentum3D momentum;
  
  Particles2D3V(Common_parameters&& parameters)
    : Particles(std::move(parameters)) {}
};

}  // namespace regular

#endif  // SRC_PARTICLES_CONCRETE_PARTICLES_2D3V_HPP
