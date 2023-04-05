#ifndef SRC_PARTICLES_PARTICLES_HPP
#define SRC_PARTICLES_PARTICLES_HPP

#include "src/pch.h"
#include "src/particles/particle/particle.hpp"
#include "src/solvers/abstract_strategies.hpp"

#include "particles_boundary_interaction.hpp"
#include "particles_builder.hpp"

class Particles {
 public:

  Particles() = default;

  Particles(Particles_builder& builder);

  const auto& get_particles() const { return particles_; }
  const auto& get_parameters() const { return parameters_; }

#if GLOBAL_DENSITY
  void add_particle(const Point& point);
#else
  void add_particle(const Point& point, double local_n);
#endif

  void push();

  /// @todo REFACTOR THIS OUT!
  friend class Manager;

  friend class Set_particles;
  friend class Copy_coordinates;
  friend class Ionize_particles;

  friend class Clone_layer_particles;
  friend class Plasma_boundary;
  friend class Beam_boundary;
  friend class Simulation_backup;

 private:
  std::string sort_name_;
  Parameters parameters_;
  std::vector<Particle> particles_;

  std::unique_ptr<Pusher> push_;
  std::unique_ptr<Interpolation> interpolation_;
  std::unique_ptr<Decomposition> decomposition_;

  std::unique_ptr<Particles_boundary> boundaries_processor_;
};

#endif  // SRC_PARTICLES_PARTICLES_HPP
