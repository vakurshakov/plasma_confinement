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
/*additional*/ string field_to_diagnose, string axis_of_this_field)
	: Fields_diagnostic(directory_path + "/" + file_name)
{
	field_ = field_to_diagnose;
	axis_  = axis_of_this_field;
	
	this->save_parameters(directory_path);
}

void whole_field::save_parameters(string directory_path)
{
	std::ofstream diagnostic_parameters_((directory_path + "/parameters.txt").c_str(), std::ios::out);
	diagnostic_parameters_ << "#TIME dt DTS" << std::endl;
	diagnostic_parameters_ << TIME << " " << dt << " " << diagnose_time_step << " " << std::endl;
	diagnostic_parameters_ << "#SIZE_X SIZE_Y" << std::endl;
	diagnostic_parameters_ << SIZE_X << " " << SIZE_Y << std::endl;
	diagnostic_parameters_ << "#sizeof(float)" << std::endl;
	diagnostic_parameters_ << sizeof(float) << std::endl;

}

void whole_field::diagnose(const v3f& F)
{
	for (int ny = 0; ny < F.size_y(); ++ny) {
	for (int nx = 0; nx < F.size_x(); ++nx) {
		if (axis_ == "x") { file_for_results_->write(F.x(ny, nx)); }
		else if (axis_ == "y") { file_for_results_->write(F.y(ny, nx)); }
		else if (axis_ == "z") { file_for_results_->write(F.z(ny, nx)); }
	}
	}
}

void whole_field::diagnose(const v3f& E, const v3f& B, const v3f& j, int t)
{
	if ((t % diagnose_time_step) == 0) {

	file_for_results_ = std::make_unique<BIN_File>(directory_path_, to_string(t));

	if ( field_ == "E" ) { this->diagnose(E); }
	else if ( field_ == "B" ) { this->diagnose(B); }
	else if ( field_ == "j" ) { this->diagnose(j); }

	file_for_results_.release();
}}