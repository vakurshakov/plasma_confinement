#ifndef SRC_DIAGNOSTICS_WHOLE_FIELD_HPP
#define SRC_DIAGNOSTICS_WHOLE_FIELD_HPP

#include "field_on_segment.hpp"

class whole_field : public field_on_segment {
 public:
  whole_field(std::string result_directory,
    const vector3_field& field, Axis component, diag_segment segment);

  void diagnose(int t) override;
};

#endif  // SRC_DIAGNOSTICS_WHOLE_FIELD_HPP
