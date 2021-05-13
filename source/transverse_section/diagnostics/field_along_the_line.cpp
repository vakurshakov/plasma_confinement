#include "./diagnostics.hpp"


void field_along_X::initialize()
{
	create_directory( path_ / "animation" );
	ofs_ << "#SIZE_X" << endl;
	ofs_ << SIZE_X << endl;
}

void field_along_Y::initialize()
{
	create_directory( path_ / "animation" );
	ofs_ << "#SIZE_Y" << endl;
	ofs_ << SIZE_Y << endl;
}

void faX_diagnose(const v3f& F, string axis, int Y, ofstream& File)
{
	float temp;
	for (int x = 0; x < F.size_x(); ++x) {
		if (axis == "x") {	
			temp = F.x(Y,x);
			File.write( (char*)&temp, sizeof(float) );
		}
		else if (axis == "y") {
			temp = F.y(Y,x);
			File.write( (char*)&temp, sizeof(float) );
		}		
		else if (axis == "z") {
			temp = F.z(Y,x);
			File.write( (char*)&temp, sizeof(float) );
		}
	}
}

void faY_diagnose(const v3f& F, string axis, int X, ofstream& File)
{	
	float temp;
	for (int y = 0; y < F.size_y(); ++y) {
		if (axis == "x") {	
			temp = F.x(y,X);
			File.write( (char*)&temp, sizeof(float) );
		}
		else if (axis == "y") {
			temp = F.y(y,X);
			File.write( (char*)&temp, sizeof(float) );
		}		
		else if (axis == "z") {
			temp = F.z(y,X);
			File.write( (char*)&temp, sizeof(float) );
		}
	}
}

void field_along_X::diagnose(const v3f& E, const v3f& B, const v3f& j)
{
	if ( field_ == "E" ) { faX_diagnose( E, axis_, Y_, OFS_ ); }
	else if ( field_ == "B" ) { faX_diagnose( B, axis_, Y_, OFS_ ); }
	else if ( field_ == "j" ) { faX_diagnose( j, axis_, Y_, OFS_ ); }
}

void field_along_Y::diagnose(const v3f& E, const v3f& B, const v3f& j)
{
	if ( field_ == "E" ) { faY_diagnose( E, axis_, X_, OFS_ ); }
	else if ( field_ == "B" ) { faY_diagnose( B, axis_, X_, OFS_ ); }
	else if ( field_ == "j" ) { faY_diagnose( j, axis_, X_, OFS_ ); }
}