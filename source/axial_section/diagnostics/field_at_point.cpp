#include "./diagnostics.hpp" 


void field_at_point::initialize() {}

void fp_diagnose(const v3f& F, string axis, int px, int py, ofstream& ofs)
{
	if (axis == "x") {	ofs << F.x(py,px) << endl; }
	else if (axis == "y") {	ofs << F.y(py,px) << endl; }		
	else if (axis == "z") { ofs << F.z(py,px) << endl; }
}

void field_at_point::diagnose(const v3f& E, const v3f& B, const v3f& j)
{
	if ( field_ == "E" ) { fp_diagnose( E, axis_, px_, py_, ofs_ ); }
	else if ( field_ == "B" ) { fp_diagnose( B, axis_, px_, py_, ofs_ ); }
	else if ( field_ == "j" ) { fp_diagnose( j, axis_, px_, py_, ofs_ ); }
}
