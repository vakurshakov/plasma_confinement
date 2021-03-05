#include "../fields.hpp"


void Fields::add_circular_current(const Species_description& sort,
	double v_inj, double Bz0, int t)
{
	double q = sort.q();
	double m = sort.m();
	double n = sort.n();
	
	/*
		double gamma = 1./sqrt(1 - v_inj*v_inj);
		int r_larm = gamma*m*v_inj/(q*Bz0)/dx;
	*/

	// NOTE: как аккуратно заполнять массив так, чтобы
	// в одно место дважды не попадать? Если это прои-
	// сходит, мы увеличиваем лишний раз ток

	double dphi;
	double ds = 1;
	double f = 1./50.;
	double vx, vy;
	
	int x, y;


	for (double r = r_larm-ds; r < (r_larm + ds)+1 ; r += ds)
	{
		dphi = dy/(r*dx);
		
		for (double phi = 0; phi < 2*M_PI; phi += dphi) {
			
			x = SIZE_X/2 + r*cos(phi);
			y = SIZE_Y/2 + r*sin(phi);
	
			vx = + v_inj*sin(phi);
			vy = - v_inj*cos(phi);


			if ( t < 1/(4.*f) ) {
				(*j_).x(y,x) += cos((r - r_larm)*2./M_PI)*sin(2*M_PI*f*t)*q*n*vx;
				(*j_).y(y,x) += cos((r - r_larm)*2./M_PI)*sin(2*M_PI*f*t)*q*n*vy;
			}
			else if ( t >= 1/(4.*f) ) {
				(*j_).x(y,x) += cos((r - r_larm)*2./M_PI)*q*n*vx;
				(*j_).y(y,x) += cos((r - r_larm)*2./M_PI)*q*n*vy;
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
