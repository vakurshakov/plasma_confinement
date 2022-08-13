#ifndef SINGLE_FIELD_HPP
#define SINGLE_FIELD_HPP

#include "src/pch.h"
#include "../vectors/vector3_field.hpp"

using std::string;


class Single_field_diagnostic {
public:
	Single_field_diagnostic(
        string directory_path,
		const vector3_field& field_to_diagnose, Axis field_axis,
        int begin_x, int end_x, int begin_y, int end_y);

	void diagnose(int t) const;
	
private:
	void save_parameters();

    const vector3_field& field_to_diagnose_;
    Axis field_axis_;

    string directory_path_;

	int begin_x_;
	int begin_y_;
	int end_x_;
	int end_y_;	
};


#endif // SINGLE_FIELD_HPP
