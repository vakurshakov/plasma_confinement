#ifndef SRC_DIAGNOSTICS_FIELD_AT_POINT_HPP
#define SRC_DIAGNOSTICS_FIELD_AT_POINT_HPP

#include "src/diagnostics/diagnostic.hpp"

#include "src/pch.h"
#include "src/vectors/basic_field.hpp"
#include "src/vectors/grid_indexes.hpp"

using std::string;

template<std::int8_t tensor_dim>
class field_at_point : public Diagnostic {
 public:
  field_at_point(
    string result_directory, string file_name,
    const basic_field<tensor_dim>& field, int comp,
    grid_indexes::index point);

  void diagnose(int t) const override;
  void save_parameters() const override;

 private:  
  const basic_field<tensor_dim>& field;
  int comp_;
  grid_indexes::index point_;
};

#include "field_at_point.inc"

#endif  // SRC_DIAGNOSTICS_FIELD_AT_POINT_HPP
