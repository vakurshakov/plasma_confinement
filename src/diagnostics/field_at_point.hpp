#ifndef SRC_DIAGNOSTICS_FIELD_AT_POINT_HPP
#define SRC_DIAGNOSTICS_FIELD_AT_POINT_HPP

#include "diagnostic.hpp"
#include "src/vectors/vector3_field.hpp"

struct diag_point {
  int x, y;

  diag_point(int x, int y)
    : x(x), y(y) {}
};

class field_at_point : public Diagnostic {
 public:
  field_at_point(std::string result_directory, std::string file_name,
    const vector3_field& field, Axis field_comp, diag_point point);

  void diagnose(int t) override;

  void save_parameters() const override;

 private:
  const vector3_field& field_;
  Axis component_;
  diag_point point_;
};

#endif  // SRC_DIAGNOSTICS_FIELD_AT_POINT_HPP
