#ifndef SRC_COMMAND_CLONE_LAYER_PARTICLES_HPP
#define SRC_COMMAND_CLONE_LAYER_PARTICLES_HPP

#include "command.hpp"

#include "src/pch.h"
#include "src/particles/particles.hpp"
#include "src/particles/particle-boundary_processor.hpp"

class Clone_layer_particles : public Command {
 public:
  Clone_layer_particles(
    Particles* const particles_inout,
    Domain_geometry geom);
  
  Clone_layer_particles(
    Particles* const particles_in,
    Particles* const particles_out,
    Domain_geometry geom);

  void execute(int /* timestep */) const override;

 private:
  Particles* const particles_in_;
  Particles* const particles_out_;

  Domain_geometry geom_;

  inline bool particle_should_cloned(const Point&) const;
  inline bool particle_on_the_left(double x, double width) const;
  inline bool particle_on_the_right(double x, double width) const;

  inline Point configure_point(const Point&) const;
};

#endif  // SRC_COMMAND_CLONE_LAYER_PARTICLES_HPP
