#ifndef DIAGNOSTICS_WHOLE_FIELD_HPP
#define DIAGNOSTICS_WHOLE_FIELD_HPP

//#################################################################################################

#include "diagnostics.hpp"

#include "src/pch.h"
#include "../vectors/vector3_field.hpp"

using std::string;


class whole_field : public Fields_diagnostic {
public:
	whole_field(string directory_path, string file_name,
	/*additional*/ string field_to_diagnose, string field_axis,
		int begin_x, int end_x, int begin_y, int end_y);

	void save_parameters(string directory_path) override;
	void diagnose(const v3f& E, const v3f& B, const v3f& j, int t) override;
	
private:
	void diagnose(const v3f& F);

	string field_, axis_;
	
	int begin_x_;
	int begin_y_;
	int end_x_;
	int end_y_;	
};


//#################################################################################################

#endif // DIAGNOSTICS_WHOLE_FIELD_HPP
