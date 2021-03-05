#include "./diagnostics.hpp" 


void fp_diagnose(const v3f& F, int axis, int px, int py, ofstream& ofs)
{
	switch (axis)
	{
		case X:
			ofs << F.x(py,px) << " ";
			break;
		case Y:
			ofs << F.y(py,px) << " ";
			break;
		case Z:
			ofs << F.z(py,px) << " ";
			break;
		default:
			return;
	}
}

void field_at_point::initialize() {}

void field_at_point::diagnose(const v3f& E, const v3f& B, const v3f& j)
{
	if ( field_ == "E" ) { fp_diagnose( E, axis_, px_, py_, ofs_ ); }
	else if ( field_ == "B" ) { fp_diagnose( B, axis_, px_, py_, ofs_ ); }
	else if ( field_ == "j" ) { fp_diagnose( j, axis_, px_, py_, ofs_ ); }
}
