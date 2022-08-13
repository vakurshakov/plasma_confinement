#include "diagnostics.hpp"

#include "src/pch.h"
#include "../vectors/vector3_field.hpp"


using std::string;
using v3f = vector3_field;


class field_along_x_axis : public Fields_diagnostic {
public:
	field_along_x_axis(string directory_path, string field_name,
	/*additional*/ string field_to_diagnose, string field_axis,
	int fixed_y, int begin_x, int end_x);

	void save_parameters(string directory_path) override;
	void diagnose(const v3f& E, const v3f& B, const v3f& j, int t) override;
	

private:
	void diagnose(const v3f& F);
	
	string field_, axis_;
	
	int fixed_y_;
	int begin_x_;
	int end_x_; 
};


class field_along_y_axis : public Fields_diagnostic {
public:
	field_along_y_axis(string directory_path, string field_name,
	/*additional*/ string field_to_diagnose, string field_axis,
		int fixed_x, int begin_y, int end_y);

	void save_parameters(string directory_path) override;
	void diagnose(const v3f& E, const v3f& B, const v3f& j, int t) override;

	
private:
	void diagnose(const v3f& F);
	
	string field_, axis_;
	
	int fixed_x_;
	int begin_y_;
	int end_y_; 
};