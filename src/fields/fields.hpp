#ifndef SRC_FIELDS_MANAGERS_FIELDS_HPP
#define SRC_FIELDS_MANAGERS_FIELDS_HPP

#include "src/pch.h"
#include "src/vectors/vector3_field.hpp"
#include "src/fields/open_boundaries_processor.hpp"
#include "src/diagnostics/diagnostics.hpp"

#include "fields_builder.hpp"

class Fields {
public:
  Fields() = default;

  using v3f_up = std::unique_ptr<vector3_field>;
  using diagnostic_up = std::unique_ptr<Fields_diagnostic>;

  Fields(Fields_builder& builder);

  vector3_field& E() { return *E_; };
  vector3_field& B() { return *B_; };
  vector3_field& J() { return *J_; };

  void propagate();
  void diagnose(int t) const;

  // additional
  friend class Magnetic_field_half_step;
  void add_Bz0(double Bz0);

private:
  v3f_up E_, B_, J_;
  Boundaries_processor_up boundary_processor_;
  std::vector<diagnostic_up> diagnostics_;
};

#endif  // SRC_FIELDS_MANAGERS_FIELDS_HPP
