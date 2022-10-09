#ifndef SRC_COMMAND_SET_PARTICLES_HPP
#define SRC_COMMAND_SET_PARTICLES_HPP

#include "command.hpp"

#include "src/pch.h"
#include "src/particles/particles.hpp"

class Coordinate_generator {
 public:
  virtual void load(double* x, double* y) = 0;
};


/**
 * @brief Command that sets particles before the main calculation cycle.
 */
class Set_particles : public Command {
 public:
  Set_particles(
    Particles* const particles,
    std::size_t num_particles_to_load,
    std::unique_ptr<Coordinate_generator> coordinate_generator,
    std::function<void(double x, double y,
      double mass, double Tx, double Ty, double Tz,
      double p0, double* px, double* py, double* pz)> load_impulse);

  void execute(int /* timestep */) const override;

 private:
  Particles* const particles_;

  std::size_t num_particles_to_load_;

  std::unique_ptr<Coordinate_generator> coordinate_generator_;

  std::function<void(double x, double y,
    double mass, double Tx, double Ty, double Tz,
    double p0, double* px, double* py, double* pz)> load_impulse_;
};

#endif  // SRC_COMMAND_SET_PARTICLES_HPP
