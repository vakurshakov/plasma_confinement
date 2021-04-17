#include "../fields.hpp"
#include <filesystem>
#include <fstream>
#include <cmath>

namespace fs = std::filesystem;

namespace qubic_shape_constants
{
	const double dr12 = dr1*dr1;
	const double dr2 = dr*dr;
 
	const double A = 2.*Bz0/(dr12*dr2); 
	const double B = -6.*Bz0/(dr1*dr*(dr1 + dr));
	const double C = 2.*Bz0/(dr1 + dr);
	const double K = -2.*Bz0/(dr2*(dr2 - dr12));
} 

namespace qsc = qubic_shape_constants;

double get_current_shape(double r)
{
	//qubic-spline shape
	
	double abs_r = fabs(r);
	double abs_r2 = abs_r*abs_r;
	double abs_r3 = abs_r*abs_r*abs_r;

	if ( abs_r <= dr1 ) {
		return qsc::A*abs_r3 + qsc::B*abs_r2 + qsc::C;
	}
	else if ( dr1 < abs_r && abs_r < dr ) {
		return qsc::K*(abs_r - dr)*(abs_r - dr)*(abs_r - dr);
	}
	else {
		return 0;
	}	
}

void Fields::add_circular_current(int t)
{
	#pragma omp parallel for shared(j_), num_threads(THREAD_NUM)
	for (int y = 0; y < (*j_).size_y(); ++y) {
	for (int x = 0; x < (*j_).size_x(); ++x) {
		
		if ( t <= t_inj ) {
			(*j_).z(y,x) += +sin(0.5*M_PI*t/double(t_inj))*get_current_shape((y - 0.5*SIZE_Y)*dy + r_larm);
			(*j_).z(y,x) += -sin(0.5*M_PI*t/double(t_inj))*get_current_shape((y - 0.5*SIZE_Y)*dy - r_larm);
		}
		else {
			(*j_).z(y,x) += +get_current_shape((y - 0.5*SIZE_Y)*dy + r_larm);
			(*j_).z(y,x) += -get_current_shape((y - 0.5*SIZE_Y)*dy - r_larm);
		}
	}
	}
}
