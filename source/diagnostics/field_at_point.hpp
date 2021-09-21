#ifndef DIAGNOSTICS_FIELD_AT_POINT_HPP
#define DIAGNOSTICS_FIELD_AT_POINT_HPP

//#################################################################################################

#include "./diagnostics.hpp"

#include <string>
#include <vector>

#include "../vectors/vector3_field.hpp"


using std::string;
using v3f = vector3_field;


class field_at_point : public Diagnostic {
public:
	field_at_point(string directory_path, string file_name,
	/*additional*/ string field_to_diagnose, string field_axis,
		int point_x, int point_y);

	void save_parameters(string directory_path) override;
	void diagnose(const v3f& E, const v3f& B, const v3f& j, int t) override;

	void diagnose(const v3f& F);
	
private:
	string field_, axis_;
	int px_, py_;
};

//#################################################################################################

#endif // DIAGNOSTICS_FIELD_AT_POINT_HPP