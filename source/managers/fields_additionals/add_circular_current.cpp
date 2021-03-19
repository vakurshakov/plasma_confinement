#include "../fields.hpp"
#include <cmath>

double get_continuous_x_current(int x, int y)
{
	// TODO: нормальный список параметров!
	double r = sqrt( x*x + y*y )*dx;

	if ( fabs( (r - r_larm) ) <= dr ) {
		return +cos(0.5*M_PI*(r - r_larm)/dr)*(0.25*M_PI*Bz0/dr) * (y*dy)/r;
	}
	else {
		return 0;
	}
}

double get_continuous_y_current(int x, int y)
{	
	double r = sqrt( x*x + y*y )*dx;

	if ( fabs( (r - r_larm) ) <= dr ) {
		return -cos(0.5*M_PI*(r - r_larm)/dr)*(0.25*M_PI*Bz0/dr) * (x*dx)/r;
	}
	else {
		return 0;
	}
}

void Fields::add_circular_current(int t)
{
	#pragma omp parallel for shared(j_), num_threads(8)
	for (int y = 0; y < (*j_).size_y(); ++y) {
	for (int x = 0; x < (*j_).size_x(); ++x) {
		if ( t < t_inj ) {
			(*j_).x(y,x) += sin(0.5*M_PI*t/t_inj)*get_continuous_x_current(((x+0.5) - 0.5*SIZE_X),
				((y+0.5) - 0.5*SIZE_Y));
			(*j_).y(y,x) += sin(0.5*M_PI*t/t_inj)*get_continuous_y_current(((x+0.5) - 0.5*SIZE_X),
				((y+0.5) - 0.5*SIZE_Y));
		}
		else {
			(*j_).x(y,x) += get_continuous_x_current(((x+0.5) - 0.5*SIZE_X),
				((y+0.5) - 0.5*SIZE_Y));
			(*j_).y(y,x) += get_continuous_y_current(((x+0.5) - 0.5*SIZE_X),
				((y+0.5) - 0.5*SIZE_Y));
		}
	}
	}
}
