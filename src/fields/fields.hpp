#ifndef SRC_FIELDS_MANAGERS_FIELDS_HPP
#define SRC_FIELDS_MANAGERS_FIELDS_HPP

#include "src/pch.h"
#include "src/vectors/vector3_field.hpp"

#include "open_boundaries_processor.hpp"
#include "fields_builder.hpp"

class Fields {
public:
  Fields() = default;

  using v3f_up = std::unique_ptr<vector3_field>;

  Fields(Fields_builder& builder);

  vector3_field& E() { return *E_; };
  vector3_field& B() { return *B_; };
  vector3_field& J() { return *J_; };

  void propagate();
  void clear_sources();

  // additional
  friend class Magnetic_field_half_step;

private:
  v3f_up E_, B_, J_;
  Boundaries_processor_up boundary_processor_;
};

void FDTD_2D(vector3_field& E, vector3_field& B, vector3_field& J);

#endif  // SRC_FIELDS_MANAGERS_FIELDS_HPP
