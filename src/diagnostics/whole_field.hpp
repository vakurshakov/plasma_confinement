#ifndef SRC_DIAGNOSTICS_WHOLE_FIELD_HPP
#define SRC_DIAGNOSTICS_WHOLE_FIELD_HPP

#include "src/diagnostics/field_on_segment.hpp"

using std::string;

template<std::int8_t tensor_dim>
class whole_field : public field_on_segment<tensor_dim> {
 public:
  whole_field(
    string directory_path, string res_dir,
    const basic_field<tensor_dim>& field, int comp,
    grid_indexes::index begin, grid_indexes::index end);

  void diagnose(int t) const override;
};

#include "whole_field.inc"

#endif  // SRC_DIAGNOSTICS_WHOLE_FIELD_HPP
