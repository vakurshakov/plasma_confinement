#include "./fields_manager.hpp" 


enum CONF { SX, SY, BOUND };

void Fields_manager::initialisation(string solver, vector<string> configuration) {

	if ( solver == "FDTD_2D" ) {
		Propogate_fields_ = FDTD_2D;			
	}


	int size_x = stoi(configuration[SX]);
	int size_y = stoi(configuration[SY]);
	string boundaries = configuration[BOUND];
	
	using ref__v3f = reflective_vector3_field;
	using per__v3f = periodic_vector3_field;
	using rhpv_v3f = rh_pv_vector3_field;
	using rvph_v3f = rv_ph_vector3_field;
	
	if ( boundaries == "reflective" ) {
		E = make_unique<ref__v3f>(size_x, size_y);
		B = make_unique<ref__v3f>(size_x, size_y);
		j = make_unique<ref__v3f>(size_x, size_y);
	}
	else if ( boundaries == "periodic" ) {
		E = make_unique<per__v3f>(size_x, size_y);
		B = make_unique<per__v3f>(size_x, size_y);
		j = make_unique<per__v3f>(size_x, size_y);
	}
	else if ( boundaries == "rh_pv" ) {
		E = make_unique<rhpv_v3f>(size_x, size_y);
		B = make_unique<rhpv_v3f>(size_x, size_y);
		j = make_unique<rhpv_v3f>(size_x, size_y);
	}
	else if ( boundaries == "rv_ph" ) {
		E = make_unique<rvph_v3f>(size_x, size_y);
		B = make_unique<rvph_v3f>(size_x, size_y);
		j = make_unique<rvph_v3f>(size_x, size_y);
	}
}

void Fields_manager::Propogate_fields() {
	Propogate_fields_(*E, *B, *j);
}

void Fields_manager::add_circular_current(const class_particles& sort,
	double v_inj_, double Bz0_, int t)
{
	double q = sort.q();
	double m = sort.m();
	double n = sort.n();
	
	double gamma = 1./sqrt(1 - v_inj_*v_inj_);
	int r_larm = gamma*m*v_inj_/(q*Bz0_)/dx;

	// NOTE: как аккуратно заполнять массив так, чтобы
	// в одно место дважды не попадать?

	double dphi, ds = 1, f = 0.05;	
	int x, y;
	double vx, vy;


	#pragma omp parallel num_threads(8)
	{
		for (int r = r_larm-ds; r < (r_larm + ds)+1; ++r)
		{
			dphi = dy/(r*dx);
			
			#pragma omp for
			for (int k = 0; k < int(2*M_PI/dphi); ++k) {
				
				x = SIZE_X/2 + r*cos(k*(dphi + 0.001));
				y = SIZE_Y/2 + r*sin(k*(dphi + 0.001));
	
				vx = + v_inj_*sin(k*dphi);
				vy = - v_inj_*cos(k*dphi);
	

				if ( t*dt < 1/(4.*f) ) {
					(*j).x(y,x) += sin(2*M_PI*f*t*dt)*q*n*vx;
					(*j).y(y,x) += sin(2*M_PI*f*t*dt)*q*n*vy;
				}
				else if ( t*dt >= 1/(4.*f) ) {
					(*j).x(y,x) += q*n*vx;
					(*j).y(y,x) += q*n*vy;
				}
			}
		}
	}
}

void Fields_manager::add_Bz0(double Bz0)
{
	#pragma omp parallel for shared(B)
	for (int y = 0; y < (*B).size_y(); ++y) {
	for (int x = 0; x < (*B).size_x(); ++x) {
		(*B).z(y,x) += Bz0;
	}
	}
}