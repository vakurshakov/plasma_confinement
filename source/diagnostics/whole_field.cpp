#include "whole_field.hpp" 

#include <string>
#include <fstream>
#include <filesystem> 

#include "../file_writers/file_interface.hpp"
#include "../vectors/vector3_field.hpp"
#include "../constants.h"

using std::string;
using v3f = vector3_field;
namespace fs = std::filesystem;


whole_field::whole_field(string directory_path, string file_name,
/*additional*/ string field_to_diagnose, string field_axis,
	int begin_x, int end_x, int begin_y, int end_y)
	: 	Fields_diagnostic(directory_path + "/" + file_name)
	{
		field_ 	 = field_to_diagnose;
		axis_  	 = field_axis;
		begin_x_ = begin_x;
		begin_y_ = begin_y;
		end_x_ 	 = end_x;
		end_y_ 	 = end_y;
		
		this->save_parameters(directory_path);
	}

void whole_field::save_parameters(string directory_path)
{
	fs::create_directories(fs::path(directory_path));
	std::ofstream diagnostic_parameters_((directory_path + "/parameters.txt").c_str(), std::ios::out);
	diagnostic_parameters_ << "#TIME, dt, DTS" << std::endl;
	diagnostic_parameters_ << TIME << " " << dt << " " << diagnose_time_step << " " << std::endl;
	diagnostic_parameters_ << "#begin_x, end_x, dx" << std::endl;
	diagnostic_parameters_ << begin_x_ << " " << end_x_ << " " << dx << " " << std::endl;
	diagnostic_parameters_ << "#begin_y, end_y, dy" << std::endl;
	diagnostic_parameters_ << begin_y_ << " " << end_y_ << " " << dy << " " << std::endl;
	diagnostic_parameters_ << "#sizeof(float)" << std::endl;
	diagnostic_parameters_ << sizeof(float) << std::endl;
}

void whole_field::diagnose(const v3f& F)
{
	for (int ny = begin_x_; ny < end_x_; ++ny) {
	for (int nx = begin_y_; nx < end_y_; ++nx) {
		if (axis_ == "x") { file_for_results_->write(F.x(ny, nx)); }
		else if (axis_ == "y") { file_for_results_->write(F.y(ny, nx)); }
		else if (axis_ == "z") { file_for_results_->write(F.z(ny, nx)); }
	}}
}

void whole_field::diagnose(const v3f& E, const v3f& B, const v3f& j, int t)
{
	if ((t % diagnose_time_step) == 0) {

	file_for_results_ = std::make_unique<BIN_File>(directory_path_, to_string(t));

	if ( field_ == "E" ) { this->diagnose(E); }
	else if ( field_ == "B" ) { this->diagnose(B); }
	else if ( field_ == "j" ) { this->diagnose(j); }
}}