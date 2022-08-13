#include "single_field.hpp" 

#include <fstream>
#include <filesystem> 

#include "../file_writers/bin_file.hpp"

using std::string;
using v3f = vector3_field;
namespace fs = std::filesystem;

Single_field_diagnostic::Single_field_diagnostic(
    string directory_path, 
    const vector3_field& field_to_diagnose, Axis field_axis,
	int begin_x, int end_x, int begin_y, int end_y)
	:   field_to_diagnose_(field_to_diagnose)
	{
        directory_path_ = directory_path;
        
        field_axis_ = field_axis;
        
		begin_x_ = begin_x;
		begin_y_ = begin_y;
		end_x_ 	 = end_x;
		end_y_ 	 = end_y;
		
        // ~ lazy initialization may be?
		this->save_parameters();
	}

void Single_field_diagnostic::save_parameters()
{
	fs::create_directories(fs::path(directory_path_));
	std::ofstream diagnostic_parameters_((directory_path_ + "/parameters.txt").c_str(), std::ios::out);
	diagnostic_parameters_ << "#TIME, dt, DTS" << std::endl;
	diagnostic_parameters_ << TIME << " " << dt << " " << diagnose_time_step << " " << std::endl;
	diagnostic_parameters_ << "#begin_x, end_x, dx" << std::endl;
	diagnostic_parameters_ << begin_x_ << " " << end_x_ << " " << dx << " " << std::endl;
	diagnostic_parameters_ << "#begin_y, end_y, dy" << std::endl;
	diagnostic_parameters_ << begin_y_ << " " << end_y_ << " " << dy << " " << std::endl;
	diagnostic_parameters_ << "#sizeof(float)" << std::endl;
	diagnostic_parameters_ << sizeof(float) << std::endl;
}

void Single_field_diagnostic::diagnose(int t) const
{
	if (t % diagnose_time_step != 0) return;

	auto file_for_results_ = std::make_unique<BIN_File>(directory_path_, to_string(t));

    for (int ny = begin_x_; ny < end_x_; ++ny) {
	for (int nx = begin_y_; nx < end_y_; ++nx) {
		
        switch (field_axis_) {
        case Axis::X:
            file_for_results_->write(field_to_diagnose_.x(ny, nx));
            break;

        case Axis::Y:
            file_for_results_->write(field_to_diagnose_.y(ny, nx));
            break;

        case Axis::Z:
            file_for_results_->write(field_to_diagnose_.z(ny, nx));
            break;
        }
    }}
}