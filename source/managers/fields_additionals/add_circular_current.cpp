#include "../fields_manager.hpp"


void Fields_manager::add_circular_current(const class_particles& sort,
	double v_inj, double Bz0, int t)
{
	double q = sort.q();
	double m = sort.m();
	double n = sort.n();
	
	double gamma = 1./sqrt(1 - v_inj*v_inj);
	int r_larm = gamma*m*v_inj/(q*Bz0)/dx;

	// NOTE: как аккуратно заполнять массив так, чтобы
	// в одно место дважды не попадать?

	double dphi;
	double ds = 1;
	double f = 1./50.;
	double vx, vy;
	
	int x, y;


	#pragma omp parallel num_threads(8)
	{
		for (int r = r_larm-ds; r < (r_larm + ds)+1; ++r)
		{
			dphi = dy/(r*dx);
			
			#pragma omp for
			for (int k = 0; k < int(2*M_PI/dphi); ++k) {
				
				x = SIZE_X/2 + r*cos(k*(dphi + 0.001));
				y = SIZE_Y/2 + r*sin(k*(dphi + 0.001));
	
				vx = + v_inj*sin(k*dphi);
				vy = - v_inj*cos(k*dphi);
	

				if ( t*dt < 1/(4.*f) ) {
					(*j_).x(y,x) += sin(2*M_PI*f*t*dt)*q*n*vx;
					(*j_).y(y,x) += sin(2*M_PI*f*t*dt)*q*n*vy;
				}
				else if ( t*dt >= 1/(4.*f) ) {
					(*j_).x(y,x) += q*n*vx;
					(*j_).y(y,x) += q*n*vy;
				}
			}
		}
	}
}

void Fields_manager::add_Bz0(double Bz0)
{
	#pragma omp parallel for shared(B_), num_threads(8)
	for (int y = 0; y < (*B_).size_y(); ++y) {
	for (int x = 0; x < (*B_).size_x(); ++x) {
		(*B_).z(y,x) += Bz0;
	}
	}
}
