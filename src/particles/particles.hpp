#ifndef SRC_PARTICLES_PARTICLES_HPP
#define SRC_PARTICLES_PARTICLES_HPP

#include "src/pch.h"
#include "src/particles/sort_common.hpp"

class Particles {
 public:
  virtual ~Particles() = default;

  Particles(Common_parameters&& parameters)
    : parameters_(std::move(parameters)) {}

  using Particle_id = size_t;
  virtual double n(Particle_id i) const { return parameters_.n; }
  virtual double q(Particle_id i) const { return parameters_.q; }
  virtual double m(Particle_id i) const { return parameters_.m; }

  const Particle_id number() { return number_in_sort; }
  const Common_parameters& get_parameters() const { return parameters_; }

protected:
  size_t number_in_sort = 0;
  Common_parameters parameters_;
};

#endif  // SRC_PARTICLES_PARTICLES_HPP
