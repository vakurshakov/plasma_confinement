#ifndef SOLVERS_H
#define SOLVERS_H
	#include "./solvers_manager.hpp" 
#endif

#include <iostream>

enum CONF { SX, SY, BOUND, NP, n0_, ni_, XY_ };
enum SOLV { PUSHER, DECOMP, FIELDS, ADDITIONAL };

//	NOTE:
//		тут есть проблема с тем, что часть функций берет значения из constant.h
//		нужно, конечно, это переделать но пока я очень не хочу


void solvers_manager::initialisation(up_v3f& E, up_v3f& B, up_v3f& j, vector<class_particles>& particles,
									 vector<string> solvers, vector<string> configuration) {
//######## configuration initialization ###########################################################

	int SIZE_X = stoi(configuration[SX]);
	int SIZE_Y = stoi(configuration[SY]);
	string boundaries = configuration[BOUND];
	
	using r_v3f 	= reflective_vector3_field;
	using p_v3f 	= periodic_vector3_field;
	using rhpv_v3f 	= rh_pv_vector3_field;
	using rvph_v3f 	= rv_ph_vector3_field;
	
	if ( boundaries == "reflective" ) {
		E = make_unique<r_v3f>(SIZE_X, SIZE_Y);
		B = make_unique<r_v3f>(SIZE_X, SIZE_Y);
		j = make_unique<r_v3f>(SIZE_X, SIZE_Y);
		x_boundary = reflective_Xboundary;
		y_boundary = reflective_Yboundary;
	}
	else if ( boundaries == "periodic" ) {
		E = make_unique<p_v3f>(SIZE_X, SIZE_Y);
		B = make_unique<p_v3f>(SIZE_X, SIZE_Y);
		j = make_unique<p_v3f>(SIZE_X, SIZE_Y);
		x_boundary = periodic_Xboundary;
		y_boundary = periodic_Yboundary;
	}
	else if ( boundaries == "rh_pv" ) {
		E = make_unique<rhpv_v3f>(SIZE_X, SIZE_Y);
		B = make_unique<rhpv_v3f>(SIZE_X, SIZE_Y);
		j = make_unique<rhpv_v3f>(SIZE_X, SIZE_Y);
		x_boundary = reflective_Xboundary;
		y_boundary = periodic_Yboundary;
	}
	else if ( boundaries == "rv_ph" ) {
		E = make_unique<rvph_v3f>(SIZE_X, SIZE_Y);
		B = make_unique<rvph_v3f>(SIZE_X, SIZE_Y);
		j = make_unique<rvph_v3f>(SIZE_X, SIZE_Y);
		x_boundary = periodic_Xboundary;
		y_boundary = reflective_Yboundary;
	}

	double n0 = stod(configuration[n0_]);
	double ni = stod(configuration[ni_]);
	double Np = stod(configuration[NP]);
	string XY_distrib = configuration[XY_];

	// for (int s = SORT; s < NUM_OF_SORTS; ++s) {
	//	if (sort[s] == "protons") {
	//
	
	protons pr_(ni);
	electrons el_(n0, Np, XY_distrib, vector2(0,0));

	particles = { pr_, el_ };
	
//#################################################################################################

//######## solvers initialization #################################################################

	if ( solvers[PUSHER] == "Boris_pusher" ) {
		Particle_push_ = Boris_pusher;
	}

	if (solvers[DECOMP] == "Esirkepov_density_decomposition" ) {
		Density_decomposition_ = Esirkepov_density_decomposition;
	}

	if ( solvers[FIELDS] == "FDTD_2D" ) {
		Propogate_fields_ = FDTD_2D;			
	}

//#################################################################################################

}

void solvers_manager::Particle_push(const class_particles& SORT, particle& PARTICLE, const up_v3f& E, const up_v3f& B) {
	Particle_push_(SORT, PARTICLE, *E, *B);
};

void solvers_manager::Density_decomposition(const class_particles& SORT, const particle& PARTICLE, const vector2& r0, up_v3f& j) {
	Density_decomposition_(SORT, PARTICLE, r0, *j);
};

void solvers_manager::Boundaries_processing(particle& PARTICLE, double size_x, double size_y) {
	x_boundary(PARTICLE, size_x);
	y_boundary(PARTICLE, size_y);
};

void solvers_manager::Propogate_fields(up_v3f& E, up_v3f& B, up_v3f& j) {
	Propogate_fields_(*E, *B, *j);
};

void solvers_manager::add_circular_current(up_v3f& J, const class_particles& sort, double v_inj_, double Bz0_, int t)
{
	double q = sort.q();
	double m = sort.m();
	double n = sort.n();
	
	double gamma = 1./sqrt(1 - v_inj_*v_inj_);
	int r_larm = gamma*m*v_inj_/(q*Bz0_)/dx;

	double dphi, ds = 1, f = 0.05;	// NOTE: как аккуратно заполнять массив так, чтобы
									// в одно место дважды не попадать?
	int x, y;
	double vx, vy;


	#pragma omp parallel num_threads(8)
	{
		for (int r = r_larm-ds; r < (r_larm + ds)+1; ++r)
		{
			dphi = dy/(r*dx);
			
			#pragma omp for
			for (int j = 0; j < int(2*M_PI/dphi); ++j) {
				
				x = SIZE_X/2 + r*cos(j*(dphi + 0.001));
				y = SIZE_Y/2 + r*sin(j*(dphi + 0.001));
	
				vx = + v_inj_*sin(j*dphi);
				vy = - v_inj_*cos(j*dphi);
	

				if ( t*dt < 1/(4.*f) ) {
					(*J).x(y,x) += sin(2*M_PI*f*t*dt)*q*n*vx;
					(*J).y(y,x) += sin(2*M_PI*f*t*dt)*q*n*vy;
				}
				else if ( t*dt >= 1/(4.*f) ) {
					(*J).x(y,x) += q*n*vx;
					(*J).y(y,x) += q*n*vy;
				}
			}
		}
	}
}

void solvers_manager::add_Bz0(up_v3f& B, double Bz0_)
{
	#pragma omp parallel for shared(B)
	for (int y = 0; y < (*B).size_y(); ++y) {
	for (int x = 0; x < (*B).size_x(); ++x) {
		(*B).z(y,x) += Bz0_;
	}
	}
}