#ifndef SRC_DIAGNOSTICS_FIELD_ON_SEGMENT_HPP
#define SRC_DIAGNOSTICS_FIELD_ON_SEGMENT_HPP

#include "diagnostic.hpp"
#include "src/vectors/vector3_field.hpp"

struct diag_segment {
  int begin[2];
  int end[2];

  diag_segment(int begin_x, int begin_y, int end_x, int end_y)
    : begin{begin_x, begin_y}, end{end_x, end_y} {}
};

class field_on_segment : public Diagnostic {
 public:
  virtual ~field_on_segment() = default;

  field_on_segment(std::string result_directory,
    const vector3_field& field, Axis component, diag_segment segment);

  void diagnose(int t) override;

  void save_parameters() const override;

 protected:
  const vector3_field& field_;
  Axis component_;
  diag_segment segment_;

 private:
  constexpr bool belongs_to_segment(int x, int y) const;
};

#endif  // SRC_DIAGNOSTICS_FIELD_ON_SEGMENT_HPP
