#ifndef SRC_SOLVERS_ESIRKEPOV_DECOMPOSITION_HPP
#define SRC_SOLVERS_ESIRKEPOV_DECOMPOSITION_HPP

#include "abstract_solvers.hpp"

#include "src/pch.h"
#include "src/fields/fields.hpp"
#include "src/utils/field_proxy.hpp"
#include "src/particles/concrete/particles_2d3v.hpp"
#include "src/vectors/vector_classes.hpp"

namespace regular {

class Esirkepov_decomposition : public Decomposition {
 public:
  Esirkepov_decomposition(const Particles2D3V&, electric_field& J);

  void process(Particles::Particle_id, const vector2& r0) override;

 private:
  void decompose_Jx(Particles::Particle_id, const vector2& r0, double particle_const);
  void decompose_Jy(Particles::Particle_id, const vector2& r0, double particle_const);
  void decompose_Jz(Particles::Particle_id, const vector2& r0, double particle_const);

  const Particles2D3V& particles;

  // We will write to it, so data race is possible
  field_proxy<3> fields_current;
};

}  // namespace regular

#endif // ESIRKEPOV_DECOMPOSITION_HPP
