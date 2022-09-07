#ifndef SRC_SOLVERS_CONCRETE_POINT_INTERPOLATION
#define SRC_SOLVERS_CONCRETE_POINT_INTERPOLATION

#include "abstract_solvers.hpp"

#include "src/pch.h"
#include "../vectors/vector_classes.hpp"

namespace regular {

struct Uniform_fields_storage {
  Uniform_fields_storage(vector3 E0, vector3 B0)
    : local_E0(E0), local_B0(B0) {}

  const vector3 local_E0;
  const vector3 local_B0;
};

/**
 * @brief This class adds to the local fields a precomputed
 * values instead of interpolating it from the grid.
 */
class Concrete_point_interpolation : public Interpolation {
 public:
  Concrete_point_interpolation(Uniform_fields_storage&&);
  void process(const vector2& r0, vector3& local_E, vector3& local_B) const override;

 private:
  Uniform_fields_storage storage_;
};

}  // namespace regular

#endif  // SRC_SOLVERS_CONCRETE_POINT_INTERPOLATION
