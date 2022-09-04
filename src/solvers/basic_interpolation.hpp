#ifndef SRC_SOLVERS_BASIC_INTERPOLATION_HPP
#define SRC_SOLVERS_BASIC_INTERPOLATION_HPP

#include "abstract_solvers.hpp"

#include "src/fields/fields.hpp"
#include "src/utils/field_proxy.hpp"
#include "src/particles/concrete/particles_2d3v.hpp"

namespace regular {

class Basic_interpolation : public Interpolation {
 public:
  Basic_interpolation(
    const Particles2D3V& particles,
    const electric_field& E, const magnetic_field& B);

  void process(const vector2& r0, vector3& local_E, vector3& local_B) const override;

private:
  const Particles2D3V& particles;
  field_proxy<3> fields_E;
  field_proxy<3> fields_B;
};

}  // namespace regular

#endif  // SRC_SOLVERS_BASIC_INTERPOLATION_HPP
