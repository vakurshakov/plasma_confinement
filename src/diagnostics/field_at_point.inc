#include "field_at_point.hpp" 

using std::string;
namespace fs = std::filesystem;

template<std::int8_t tensor_dim>
field_at_point<tensor_dim>::field_at_point(
    string result_directory, string file_name,
    const basic_field<tensor_dim>& field, int comp,
    grid_indexes::index point)
    : Diagnostic(result_directory),
      field(field), comp_(comp), point_(point) {	
  save_parameters();
  file_for_results_ = std::make_unique<BIN_File>(
    result_directory, file_name);
}

template<std::int8_t tensor_dim>
void field_at_point<tensor_dim>::save_parameters() const {
  fs::create_directories(fs::path(result_directory_));
  std::ofstream params_file(
    (this->result_directory_ + "/parameters.txt").c_str(), std::ios::out);
  
  params_file << "#TIME dt DTS\n";
  params_file << TIME << " " << dt << " " << diagnose_time_step << " \n";
  params_file << "#PX PY\n";
  params_file << point_.x << " " << point_.y << " \n";
  params_file << "#sizeof(float)\n";
  params_file << sizeof(float) << std::endl;
}

template<std::int8_t tensor_dim>
void field_at_point<tensor_dim>::diagnose(int t) const {
  file_for_results_->write(field(comp_, point_.x, point_.y));
  file_for_results_->flush();
}