#include "whole_field.hpp"
#include "src/file_writers/bin_file.hpp"

namespace fs = std::filesystem;

whole_field::whole_field(std::string result_directory,
    const vector3_field& field, Axis component, diag_segment segment)
    : field_on_segment(result_directory, field, component, segment) {}

void whole_field::diagnose(int t) {
  PROFILE_FUNCTION();

  if (t % diagnose_time_step != 0) return;

  file_for_results_ = std::make_unique<BIN_File>(
    result_directory_, std::to_string(t));

  for (int ny = segment_.begin[Y]; ny < segment_.end[Y]; ++ny) {
  for (int nx = segment_.begin[X]; nx < segment_.end[X]; ++nx) {
    file_for_results_->write(field_(component_, ny, nx));
  }}

  file_for_results_->close();
}
