#include "whole_field.hpp"

#include "src/file_writers/bin_file.hpp"

using std::string;
namespace fs = std::filesystem;

template<std::int8_t tensor_dim>
whole_field<tensor_dim>::whole_field(
    string directory_path, string res_dir,
    const basic_field<tensor_dim>& field, int comp,
    grid_indexes::index begin, grid_indexes::index end)
    : field_on_segment<tensor_dim>(
      directory_path, res_dir, field, comp, begin, end) {
}

template<std::int8_t tensor_dim>
void whole_field<tensor_dim>::diagnose(int t) const {
  if (t % diagnose_time_step != 0) return;

  // We should specify `this` because inheritance
  // from a template class hides its members.
  this->file_for_results_ = std::make_unique<BIN_File>(
    this->result_directory_, to_string(t));

  for (const auto& g : grid_indexes(this->begin_, this->end_)) {
    this->file_for_results_->write(this->field(this->comp_, g));
  }
}