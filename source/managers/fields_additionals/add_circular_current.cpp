#include "../fields.hpp"
#include <cmath>

double get_continuous_x_current(int x, int y)
{
	// TODO: нормальный список параметров!
	// это задаёт просто циркулярные токи везде

	double r = sqrt( x*x + y*y )*dx;
	if ( fabs( (r - r_larm) ) < 0.2 ) {
		return +cos(0.5*M_PI*(r - r_larm)/0.2)*e*ni*v_inj * (y*dx)/r;
	}
	else {
		return 0;
	}
}

double get_continuous_y_current(int x, int y)
{
	double r = sqrt( x*x + y*y )*dx;
	if ( fabs( (r - r_larm) ) < 1. ) {
		return -cos(0.5*M_PI*(r - r_larm))*e*ni*v_inj * (x*dx)/r;
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
		if ( t < 1/(4.*f) ) {
			(*j_).x(y,x) += sin(2*M_PI*f*t)*get_continuous_x_current((x+0.5),(y+0.5));
			(*j_).y(y,x) += sin(2*M_PI*f*t)*get_continuous_y_current((x+0.5),(y+0.5));
		}
		else if ( t >= 1/(4.*f) ) {
			(*j_).x(y,x) += get_continuous_x_current((x+0.5),(y+0.5));
			(*j_).y(y,x) += get_continuous_y_current((x+0.5),(y+0.5));
		}
	}
	}
}

void Fields::add_Bz0(double Bz0)
{
	#pragma omp parallel for shared(B_), num_threads(8)
	for (int y = 0; y < (*B_).size_y(); ++y) {
	for (int x = 0; x < (*B_).size_x(); ++x) {
		(*B_).z(y,x) += Bz0;
	}
	}
}
