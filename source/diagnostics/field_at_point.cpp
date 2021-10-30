#include "field_at_point.hpp" 

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <filesystem>

#include "../vectors/vector3_field.hpp"
#include "../constants.h"

using std::string;
using v3f = vector3_field;
namespace fs = std::filesystem;


field_at_point::field_at_point(string directory_path, string file_name,
/*additional*/ string field_to_diagnose, string axis_of_this_field,
	int point_x, int point_y)
	: Fields_diagnostic(directory_path + "/" + file_name)
	{
		field_ = field_to_diagnose;
		axis_  = axis_of_this_field;
		px_    = point_x;
		py_    = point_y;

		this->save_parameters(directory_path);
		file_for_results_ = std::make_unique<BIN_File>(directory_path_, "total");
	}

void field_at_point::save_parameters(string directory_path)
{
	fs::create_directories(fs::path(directory_path));
	std::ofstream diagnostic_parameters_((directory_path + "/parameters.txt").c_str(), std::ios::out);
	diagnostic_parameters_ << "#TIME dt DTS" << std::endl;
	diagnostic_parameters_ << TIME << " " << dt << " " << diagnose_time_step << " " << std::endl;
	diagnostic_parameters_ << "#PX PY" << std::endl;
	diagnostic_parameters_ << px_ << " " << py_ << " " << std::endl;
	diagnostic_parameters_ << "#sizeof(float)" << std::endl;
	diagnostic_parameters_ << sizeof(float) << std::endl;
}

void field_at_point::diagnose(const v3f& F)
{
	if (axis_ == "x") { file_for_results_->write(F.x(py_, px_)); }
	else if (axis_ == "y") {file_for_results_->write(F.y(py_, px_)); }		
	else if (axis_ == "z") {file_for_results_->write(F.z(py_, px_)); }
}
	

void field_at_point::diagnose(const v3f& E, const v3f& B, const v3f& j, int t)
{
	if ( field_ == "E" ) { this->diagnose(E); }
	else if ( field_ == "B" ) { this->diagnose(B); }
	else if ( field_ == "j" ) { this->diagnose(j); }
}
