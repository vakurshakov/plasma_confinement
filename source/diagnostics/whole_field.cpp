#include "./diagnostics.hpp" 


void wf_diagnose(const v3f& F, string axis, ofstream& ofs)
{
	for (int y = 0; y < F.size_y(); ++y) {
	for (int x = 0; x < F.size_x(); ++x) {
		if (axis == "x") {	ofs << F.x(y,x) << " "; }
		else if (axis == "y") {	ofs << F.y(y,x) << " "; }		
		else if (axis == "z") { ofs << F.z(y,x) << " "; }
	}
		ofs << "\t";
	}
	ofs << std::endl;
}

void whole_field::initialize()
{
	ofs_ << SIZE_X << " " << SIZE_Y << " " << endl;
}

void whole_field::diagnose(const v3f& E, const v3f& B, const v3f& j)
{
	if ( field_ == "E" ) { wf_diagnose( E, axis_, ofs_ ); }
	else if ( field_ == "B" ) { wf_diagnose( B, axis_, ofs_ ); }
	else if ( field_ == "j" ) { wf_diagnose( j, axis_, ofs_ ); }
}