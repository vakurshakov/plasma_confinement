#include "./diagnostics.hpp"


void faX_diagnose(const v3f& F, string axis, int Y, ofstream& ofs)
{
	for (int x = 0; x < F.size_x(); ++x) {
		if (axis == "x") {	ofs << F.x(Y,x) << " "; }
		else if (axis == "y") {	ofs << F.y(Y,x) << " "; }		
		else if (axis == "z") { ofs << F.z(Y,x) << " "; }
	}
	ofs << std::endl;
}

void faY_diagnose(const v3f& F, string axis, int X, ofstream& ofs)
{	
	for (int y = 0; y < F.size_y(); ++y) {
		if (axis == "x") {	ofs << F.x(y,X) << " "; }
		else if (axis == "y") {	ofs << F.y(y,X) << " "; }		
		else if (axis == "z") { ofs << F.z(y,X) << " "; }
	}
	ofs << std::endl;
}

void field_along_X::initialize()
{
	ofs_ << SIZE_X << endl;
}

void field_along_X::diagnose(const v3f& E, const v3f& B, const v3f& j)
{
	if ( field_ == "E" ) { faX_diagnose( E, axis_, Y_, ofs_ ); }
	else if ( field_ == "B" ) { faX_diagnose( B, axis_, Y_, ofs_ ); }
	else if ( field_ == "j" ) { faX_diagnose( j, axis_, Y_, ofs_ ); }
}

void field_along_Y::initialize()
{
	ofs_ << SIZE_Y << endl;
}

void field_along_Y::diagnose(const v3f& E, const v3f& B, const v3f& j)
{
	if ( field_ == "E" ) { faY_diagnose( E, axis_, X_, ofs_ ); }
	else if ( field_ == "B" ) { faY_diagnose( B, axis_, X_, ofs_ ); }
	else if ( field_ == "j" ) { faY_diagnose( j, axis_, X_, ofs_ ); }
}