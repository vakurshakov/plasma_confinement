#include "./diagnostics.hpp" 


void field_at_point::initialize() {}

void fp_diagnose(const v3f& F, string axis, int px, int py, ofstream& File)
{
	float temp;
	if (axis == "x") 	{
		temp = F.x(py,px);
		File.write( (char*)&temp, sizeof(float) ); 
	}
	else if (axis == "y") {
		temp = F.y(py,px);
		File.write( (char*)&temp, sizeof(float) );
	}		
	else if (axis == "z") {
		temp = F.z(py,px);
		File.write( (char*)&temp, sizeof(float) );
	}
}
	

void field_at_point::diagnose(const v3f& E, const v3f& B, const v3f& j, int t)
{
	if ( field_ == "E" ) { fp_diagnose( E, axis_, px_, py_, OFS_ ); }
	else if ( field_ == "B" ) { fp_diagnose( B, axis_, px_, py_, OFS_ ); }
	else if ( field_ == "j" ) { fp_diagnose( j, axis_, px_, py_, OFS_ ); }
}
