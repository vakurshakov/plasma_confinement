#include "diagnostics.hpp"

#include <string>

#include "../vectors/vector3_field.hpp"


using std::string;
using v3f = vector3_field;


class field_along_x_axis : public Fields_diagnostic {
public:
	field_along_x_axis(string directory_path, string field_name,
	/*additional*/ string field_to_diagnose, string field_axis, int fixed_y);

	void save_parameters(string directory_path) override;
	void diagnose(const v3f& E, const v3f& B, const v3f& j, int t) override;
	
	void diagnose(const v3f& F);

private:
	string field_, axis_;
	int fixed_y_; 
};


class field_along_y_axis : public Fields_diagnostic {
public:
	field_along_y_axis(string directory_path, string field_name,
	/*additional*/ string field_to_diagnose, string field_axis, int fixed_x);

	void save_parameters(string directory_path) override;
	void diagnose(const v3f& E, const v3f& B, const v3f& j, int t) override;

	void diagnose(const v3f& F);
	
private:
	string field_, axis_;
	int fixed_x_; 
};