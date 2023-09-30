#include "field_on_segment.hpp"
#include "src/file_writers/bin_file.hpp"

namespace fs = std::filesystem;

field_on_segment::field_on_segment(
    std::string result_directory,
    const vector3_field& field, Axis component, diag_segment segment)
    : Diagnostic(result_directory),
      field_(field), component_(component), segment_(segment) {
  save_parameters();
}

void field_on_segment::save_parameters() const {
  fs::create_directories(fs::path(result_directory_));

  std::ofstream param_file((result_directory_ +
    "/parameters.txt").c_str(), std::ios::out);

  param_file << "#TIME, dt, DTS\n";
  param_file << TIME << " " << dt << " " << diagnose_time_step << " \n";
  param_file << "#begin_x, end_x, dx\n";
  param_file << segment_.begin[X] << " " << segment_.end[X] << " " << dx << " \n";
  param_file << "#begin_y, end_y, dy\n";
  param_file << segment_.begin[Y] << " " << segment_.end[Y] << " " << dy << " \n";
  param_file << "#sizeof(float)\n";
  param_file << sizeof(float) << std::endl;
}

void field_on_segment::diagnose(int t) {
  PROFILE_FUNCTION();

  // if (t % diagnose_time_step != 0) return;

  file_for_results_ = std::make_unique<BIN_File>(
    BIN_File::from_timestep(result_directory_, t));

  for (int ny = 0; ny < field_.size_y(); ++ny) {
  for (int nx = 0; nx < field_.size_x(); ++nx) {
    if (belongs_to_segment(nx, ny)) {
      file_for_results_->write(field_(ny, nx, component_));
    }
  }}
}

constexpr bool field_on_segment::belongs_to_segment(int x, int y) const {
  return
    (segment_.begin[X] <= x && x <= segment_.end[X]) &&
    (segment_.begin[Y] <= y && y <= segment_.end[Y]) &&
    ((y - segment_.begin[Y]) * (segment_.end[X] - segment_.begin[X]) ==
     (x - segment_.begin[X]) * (segment_.end[Y] - segment_.begin[Y]));
}
