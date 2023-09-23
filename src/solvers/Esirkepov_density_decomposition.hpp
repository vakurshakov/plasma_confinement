#ifndef ESIRKEPOV_DENSITY_DECOMPOSITION_HPP
#define ESIRKEPOV_DENSITY_DECOMPOSITION_HPP

#include "abstract_strategies.hpp"

#include "src/pch.h"
#include "src/particles/particle/particle.hpp"
#include "src/vectors/vector3_field.hpp"

class Esirkepov_density_decomposition : public Decomposition {
 public:
  Esirkepov_density_decomposition(const Parameters& parameters, vector3_field& J);
  void process(const Particle&, const vector2& r0) override;

 private:
  using v3f = vector3_field;
  void decompose_x(const Particle&, const vector2& r0, double n, double q, int nx, int ny, v3f& temp_J);
  void decompose_y(const Particle&, const vector2& r0, double n, double q, int nx, int ny, v3f& temp_J);
  void decompose_z(const Particle&, const vector2& r0, double n, double q, int nx, int ny, v3f& temp_J);

  const int Np_;
  vector3_field& J_;
};

#endif // ESIRKEPOV_DENSITY_DECOMPOSITION_HPP
