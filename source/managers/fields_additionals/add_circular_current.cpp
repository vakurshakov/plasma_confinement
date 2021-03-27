#include "../fields.hpp"
#include <cmath>

double get_continuous_x_current(double x, double y)
{
	// TODO: нормальный список параметров!
	double r = sqrt( x*x + y*y );

	if ( fabs( (r - r_larm) ) <= dr ) {
		return +cos(0.5*M_PI*(r - r_larm)/dr)*(0.25*M_PI*Bz0/dr) * y/r;
	}
	else {
		return 0;
	}
}

double get_continuous_y_current(double x, double y)
{	
	double r = sqrt( x*x + y*y );

	if ( fabs( (r - r_larm) ) <= dr ) {
		return -cos(0.5*M_PI*(r - r_larm)/dr)*(0.25*M_PI*Bz0/dr) * x/r;
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
			(*j_).x(y,x) += sin(0.5*M_PI*t/double(t_inj))*get_continuous_x_current(((x+0.5) - 0.5*SIZE_X)*dx,
				(y - 0.5*SIZE_Y)*dy);
			(*j_).y(y,x) += sin(0.5*M_PI*t/double(t_inj))*get_continuous_y_current((x - 0.5*SIZE_X)*dx,
				((y+0.5) - 0.5*SIZE_Y)*dy);
		}
		else {
			(*j_).x(y,x) += get_continuous_x_current(((x+0.5) - 0.5*SIZE_X)*dx,
				(y - 0.5*SIZE_Y)*dy);
			(*j_).y(y,x) += get_continuous_y_current((x - 0.5*SIZE_X)*dx,
				((y+0.5) - 0.5*SIZE_Y)*dy);
		}
	}
	}
}
