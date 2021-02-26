#include "./diagnostics.hpp" 


void wf_diagnose(const v3f& F, int axis, ofstream& ofs)
{
	for (int y = 0; y < F.size_y(); ++y) {
	for (int x = 0; x < F.size_x(); ++x) {
		switch (axis)
		{
			case X:
				ofs << F.x(y,x) << " ";
				break;
			case Y:
				ofs << F.y(y,x) << " ";
				break;
			case Z:
				ofs << F.z(y,x) << " ";
				break;
			default:
				return;
		}
	}
		ofs << "\t";
	}
	ofs << std::endl;
}

void whole_field::initialization()
{
	ofs_ << SIZE_X << " " << SIZE_Y << " " << endl;
}

void whole_field::diagnose(const v3f& E, const v3f& B, const v3f& j)
{
	if ( field_ == "E" ) { wf_diagnose( E, axis_, ofs_ ); }
	else if ( field_ == "B" ) { wf_diagnose( B, axis_, ofs_ ); }
	else if ( field_ == "j" ) { wf_diagnose( j, axis_, ofs_ ); }
}