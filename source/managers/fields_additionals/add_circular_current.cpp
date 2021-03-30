#include "../fields.hpp"
#include <cmath>

double get_current_shape(double r)
{
	//quadratic-spline shape

	double dr12 = dr1*dr1;
	double dr2 = dr*dr;
 
	double A = 2.*Bz0/(dr12*dr2); 
	double B = -6.*Bz0/(dr1*dr*(dr1 + dr));
	double C = 2.*Bz0/(dr1 + dr);
	double K = -2.*Bz0/(dr2*(dr2 - dr12));
	
	double abs_r = fabs(r);
	double abs_r2 = abs_r*abs_r;
	double abs_r3 = abs_r*abs_r*abs_r;

	if ( abs_r <= dr1 ) {
		return A*abs_r3 + B*abs_r2 + C;
	}
	else if ( dr1 < abs_r && abs_r < dr ) {
		return K*(abs_r - dr)*(abs_r - dr)*(abs_r - dr);
	}
	else {
		return 0;
	}	


/*
	if ( fabs(r) <= dr ) {
		return Bz0/(2*dr)*( 1 + cos(M_PI*r/dr) );
	}
	else {
		return 0;
	}
*/

}

double get_continuous_x_current(double x, double y)
{
	// TODO: нормальный список параметров!
	double r = sqrt( x*x + y*y );
	return +get_current_shape(r - r_larm) * y/r;
}

double get_continuous_y_current(double x, double y)
{	
	double r = sqrt( x*x + y*y );
	return -get_current_shape(r - r_larm) * x/r;
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
