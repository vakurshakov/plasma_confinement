#ifndef PARTICLES_DISTRIBUTION_HPP
#define PARTICLES_DISTRIBUTION_HPP

#include "src/command/set_particles.hpp"

namespace transition_layer {

class Random_coordinate_generator : public Coordinate_generator {
 public:
  Random_coordinate_generator() = default;

  virtual int get_particles_number() const;

  void load(double* x, double* y) override;

 private:
  double get_probability(double x) const;
};

class Boundary_coordinate_generator : public Random_coordinate_generator {
 public:
  Boundary_coordinate_generator() = default;

  int get_particles_number() const override;

  void load(double* x, double* y) override;
};

void load_monoenergetic_impulse(double x, double y,
  double mass, double Tx, double Ty, double Tz,
  double p0, double* px, double* py, double* pz);

}  // namespace transition_layer

#endif  // PARTICLES_DISTRIBUTION_HPP
