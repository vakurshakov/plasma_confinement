#ifndef SRC_PARTICLES_PARTICLES_HPP
#define SRC_PARTICLES_PARTICLES_HPP

#include "src/pch.h"
#include "src/particles/particle/particle.hpp"
#include "src/solvers/abstract_strategies.hpp"

#include "particle-boundary_processor.hpp"
#include "particles_builder.hpp"

class Particles {
 public:

  Particles() = default;

  Particles(Particles_builder& builder);

  const auto& get_name() const { return sort_name_; }
  const auto& get_particles() const { return particles_; }
  const auto& get_parameters() const { return parameters_; }

  void add_particle(const Point& point, ...);

  void push();

  /// @todo refactor this out
  friend class Set_particles;
  friend class Copy_coordinates;
  friend class Ionize_particles;

  friend class Clone_layer_particles;
  friend class Plasma_boundary_processor;
  friend class Beam_boundary_processor;

 private:
  std::string sort_name_;
  Parameters parameters_;
  std::vector<Particle> particles_;

  std::unique_ptr<Pusher> push_;
  std::unique_ptr<Interpolation> interpolation_;
  std::unique_ptr<Decomposition> decomposition_;

  std::unique_ptr<Particle_boundary_processor> boundaries_processor_;
};

#endif  // SRC_PARTICLES_PARTICLES_HPP
