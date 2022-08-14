#include "field_on_segment.hpp"

#include "src/file_writers/bin_file.hpp"

using std::string;
namespace fs = std::filesystem;

template<std::int8_t tensor_dim>
field_on_segment<tensor_dim>::field_on_segment(
    string directory_path, string res_dir,
    const basic_field<tensor_dim>& field, int comp,
    grid_indexes::index begin, grid_indexes::index end)
    : Diagnostic(directory_path + "/" + res_dir + "/"),
      field(field), comp_(comp), begin_(begin), end_(end) {	
  this->save_parameters();
}

template<std::int8_t tensor_dim>
void field_on_segment<tensor_dim>::save_parameters() const {
  fs::create_directories(fs::path(result_directory_));  
  std::ofstream param_file(
    (result_directory_ + "/parameters.txt").c_str(), std::ios::out);

  param_file << "#TIME, dt, DTS\n";
  param_file << TIME << " " << dt << " " << diagnose_time_step << " \n";
  param_file << "#begin_x, end_x, dx\n";
  param_file << begin_.x << " " << end_.x << " " << dx << " \n";
  param_file << "#begin_y, end_y, dy\n";
  param_file << begin_.y << " " << end_.y << " " << dy << " \n";
  param_file << "#sizeof(float)\n";
  param_file << sizeof(float) << std::endl;
}

template<std::int8_t tensor_dim>
void field_on_segment<tensor_dim>::diagnose(int t) const {
  if (t % diagnose_time_step != 0) return;

  file_for_results_ = std::make_unique<BIN_File>(
    result_directory_, to_string(t));

  for (const auto& g : field.indexes(comp_)) {
    if (belongs_to_segment(g)) {
      file_for_results_->write(field(comp_, g));
    }
  }
}

// TODO: Rewrite to various dimensions
template<std::int8_t tensor_dim>
bool field_on_segment<tensor_dim>::belongs_to_segment(
    grid_indexes::index test_g) const {
  return
    (begin_.x < test_g.x && test_g.x < end_.x) &&
    (begin_.y < test_g.y && test_g.y < end_.y) &&
    ((test_g.y - begin_.y) * (end_.x - begin_.x) ==
    (end_.y - begin_.y) * (test_g.x - begin_.x));
}