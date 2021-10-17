#include "field_along_the_line.hpp"

#include <string>
#include <fstream>
#include <filesystem> 

#include "../vectors/vector3_field.hpp"
#include "../constants.h"


using std::string;
using v3f = vector3_field;
namespace fs = std::filesystem;


field_along_x_axis::field_along_x_axis(string directory_path, string name,
/*additional*/ string field_to_diagnose, string axis_of_this_field, int fixed_y)
	: Fields_diagnostic(directory_path, name)
{
	field_ = field_to_diagnose;
	axis_ = axis_of_this_field;

	fixed_y_ = fixed_y;

	this->save_parameters(directory_path);
	fs::create_directory( fs::path(directory_path + "/animation") );
}

void field_along_x_axis::save_parameters(string directory_path)
{
	std::ofstream diagnostic_parameters_((directory_path + "/parameters.txt").c_str(), std::ios::out);
	diagnostic_parameters_ << "#TIME dt DTS" << std::endl;
	diagnostic_parameters_ << TIME << " " << dt << " " << diagnose_time_step << " " << std::endl;
	diagnostic_parameters_ << "#PY SIZE_X"<< std::endl;
	diagnostic_parameters_ << fixed_y_ << " " << SIZE_X << std::endl;

	switch ( file_for_results_->get_type() ) {
		case file_type::txt:
			break;

		case file_type::bin:
			diagnostic_parameters_ << "#sizeof(float)" << std::endl;
			diagnostic_parameters_ << sizeof(float) << std::endl;
			break;

		case file_type::hdf5:
			break;
	}
}


void field_along_x_axis::diagnose(const v3f& F)
{
	for (int nx = 0; nx < F.size_x(); ++nx) {
		if (axis_ == "x") { file_for_results_->write(F.x(fixed_y_, nx)); }
		else if (axis_ == "y") { file_for_results_->write(F.y(fixed_y_, nx)); }		
		else if (axis_ == "z") { file_for_results_->write(F.z(fixed_y_, nx)); }
	}
}


void field_along_x_axis::diagnose(const v3f& E, const v3f& B, const v3f& j, int t)
{
	if ((t % diagnose_time_step) == 0 ) {
		if ( field_ == "E" ) { this->diagnose(E); }
		else if ( field_ == "B" ) { this->diagnose(B); }
		else if ( field_ == "j" ) { this->diagnose(j); }

	} 
}


//-------------------------------------------------------------------------------------------------


field_along_y_axis::field_along_y_axis(string directory_path, string name,
/*additional*/ string field_to_diagnose, string axis_of_this_field, int fixed_x)
	: Fields_diagnostic(directory_path, name)
{
	field_ = field_to_diagnose;
	axis_ = axis_of_this_field;

	fixed_x_ = fixed_x;

	this->save_parameters(directory_path);
	fs::create_directory( fs::path(directory_path + "/animation") );
}

void field_along_y_axis::save_parameters(string directory_path)
{
	std::ofstream diagnostic_parameters_((directory_path + "/parameters.txt").c_str(), std::ios::out);
	diagnostic_parameters_ << "#TIME dt DTS" << std::endl;
	diagnostic_parameters_ << TIME << " " << dt << " " << diagnose_time_step << " " << std::endl;
	diagnostic_parameters_ << "#PX SIZE_Y" << std::endl;
	diagnostic_parameters_ << fixed_x_ << " " << SIZE_Y << std::endl;

	switch ( file_for_results_->get_type() ) {
		case file_type::txt:
			break;

		case file_type::bin:
			diagnostic_parameters_ << "#sizeof(float)" << std::endl;
			diagnostic_parameters_ << sizeof(float) << std::endl;
			break;

		case file_type::hdf5:
			break;
	}
}


void field_along_y_axis::diagnose(const v3f& F)
{
	for (int ny = 0; ny < F.size_x(); ++ny) {
		if (axis_ == "x") { file_for_results_->write(F.x(ny, fixed_x_)); }
		else if (axis_ == "y") { file_for_results_->write(F.y(ny, fixed_x_)); }		
		else if (axis_ == "z") { file_for_results_->write(F.z(ny, fixed_x_)); }
	}
}


void field_along_y_axis::diagnose(const v3f& E, const v3f& B, const v3f& j, int t)
{
	if ((t % diagnose_time_step) == 0 ) {
		if ( field_ == "E" ) { this->diagnose(E); }
		else if ( field_ == "B" ) { this->diagnose(B); }
		else if ( field_ == "j" ) { this->diagnose(j); }

	} 
}