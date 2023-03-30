#include "field_at_point.hpp"
#include "src/file_writers/bin_file.hpp"

namespace fs = std::filesystem;

field_at_point::field_at_point(
    std::string result_directory, std::string file_name,
    const vector3_field& field, Axis field_comp, diag_point point)
    : Diagnostic(result_directory),
      field_(field), component_(field_comp),
      point_(point) {
  save_parameters();

#if !START_FROM_BACKUP
  file_for_results_ = std::make_unique<BIN_File>(result_directory, file_name);

#else
  file_for_results_ = std::make_unique<BIN_File>(
    BIN_File::from_backup(result_directory_, file_name, sizeof(float)));

#endif
}

void field_at_point::save_parameters() const {
  fs::create_directories(fs::path(result_directory_));

  std::ofstream diagnostic_parameters_((result_directory_ +
    "/parameters.txt").c_str(), std::ios::out);

  diagnostic_parameters_ << "#TIME dt DTS\n";
  diagnostic_parameters_ << TIME << " " << dt << " " << diagnose_time_step << " \n";
  diagnostic_parameters_ << "#PX PY\n";
  diagnostic_parameters_ << point_.x << " " << point_.y << " \n";
  diagnostic_parameters_ << "#sizeof(float)\n";
  diagnostic_parameters_ << sizeof(float) << std::endl;
}

void field_at_point::diagnose(int t) {
  PROFILE_FUNCTION();

  file_for_results_->write(field_(point_.y, point_.x).vec[component_]);

  if (t % diagnose_time_step == 0)
    file_for_results_->flush();
}
