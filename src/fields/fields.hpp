#ifndef SRC_FIELDS_FIELDS_HPP
#define SRC_FIELDS_FIELDS_HPP

#include "src/pch.h"
#include "src/fields/em_fields.hpp"
#include "src/vectors/vector_classes.hpp"
#include "src/diagnostics/diagnostic.hpp"

namespace regular {

class Electromagnetic_field {
 public:
  electric_field E;
  magnetic_field B;
  electric_field J;

  Electromagnetic_field() = default;
  Electromagnetic_field(int size_x, int size_y);

  void propagate();
  void diagnose(int t) const;

  void set_uniform_E(vector3 uni_E);
  void set_uniform_B(vector3 uni_B);

 private:
  using diagnostic_up = std::unique_ptr<Diagnostic>;
  std::vector<diagnostic_up> diagnostics_;
};

}  // namespace regular

#endif  // SRC_FIELDS_FIELDS_HPP
