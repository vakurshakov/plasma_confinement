#ifndef DIAGNOSTICS_WHOLE_FIELD_HPP
#define DIAGNOSTICS_WHOLE_FIELD_HPP

//#################################################################################################

#include "diagnostics.hpp"

#include <string>

#include "../vectors/vector3_field.hpp"

using std::string;


class whole_field : public Fields_diagnostic {
public:
	whole_field(string directory_path, string file_name,
	/*additional*/ string field_to_diagnose, string field_axis);

	void save_parameters(string directory_path) override;
	void diagnose(const v3f& E, const v3f& B, const v3f& j, int t) override;
	
	void diagnose(const v3f& F);
	
private:
	string field_, axis_;
};


//#################################################################################################

#endif // DIAGNOSTICS_WHOLE_FIELD_HPP
