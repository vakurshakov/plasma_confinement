#ifndef SRC_DIAGNOSTICS_FIELD_ALONG_LINE_HPP
#define SRC_DIAGNOSTICS_FIELD_ALONG_LINE_HPP

#include "src/diagnostics/diagnostic.hpp"

#include "src/pch.h"
#include "src/vectors/basic_field.hpp"
#include "src/vectors/grid_indexes.hpp"

using std::string;

template<std::int8_t tensor_dim>
class field_on_segment : public Diagnostic {
 public:
  virtual ~field_on_segment() = default;

  field_on_segment(
    string directory_path, string res_dir,
    const basic_field<tensor_dim>& field, int comp,
    grid_indexes::index begin, grid_indexes::index end);

  void diagnose(int t) const override;
  void save_parameters() const override;

 protected:
  const basic_field<tensor_dim>& field;
  int comp_;
  grid_indexes::index begin_;
  grid_indexes::index end_;

 private:
  bool belongs_to_segment(grid_indexes::index test_g) const;
};

#include "field_on_segment.inc"

#endif  // SRC_DIAGNOSTICS_FIELD_ALONG_LINE_HPP
