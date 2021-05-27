#include "./diagnostics.hpp" 


void whole_field::initialize()
{
	create_directory(path_ / "animation");
	ofs_ << "#SIZE_X SIZE_Y" << endl;
	ofs_ << SIZE_X << " " << SIZE_Y << " " << endl;
}

void wf_diagnose(const v3f& F, string axis, ofstream& File)
{
	float temp;
	for (int y = 0; y < F.size_y(); ++y) {
	for (int x = 0; x < F.size_x(); ++x) {
		if (axis == "x") 	{
			temp = F.x(y,x);
			File.write( (char*)&temp, sizeof(float) ); 
		}
		else if (axis == "y") {
			temp = F.y(y,x);
			File.write( (char*)&temp, sizeof(float) );
		}		
		else if (axis == "z") {
			temp = F.z(y,x);
			File.write( (char*)&temp, sizeof(float) );
		}
	}
	}
}

void whole_field::diagnose(const v3f& E, const v3f& B, const v3f& j)
{
	if ( field_ == "E" ) { wf_diagnose( E, axis_, OFS_ ); }
	else if ( field_ == "B" ) { wf_diagnose( B, axis_, OFS_ ); }
	else if ( field_ == "j" ) { wf_diagnose( j, axis_, OFS_ ); }
}