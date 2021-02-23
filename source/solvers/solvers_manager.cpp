#ifndef SOLVERS_H
#define SOLVERS_H
	#include "./solvers_manager.hpp" 
#endif

#include <iostream>

enum CONF { SX, SY, BOUND, NP, np_, XY_ };
enum SOLV { PUSHER, DECOMP, FIELDS, ADDITIONAL };

//	NOTE:
//		тут есть проблема с тем, что часть функций берет значения из constant.h
//		нужно, конечно, это переделать но пока я очень не хочу


void solvers_manager::initialisation(up_v3f& E, up_v3f& B, up_v3f& j, vector<class_particles> particles,
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

	double np = stod(configuration[np_]);
	double Np = stod(configuration[NP]);
	string XY_distrib = configuration[XY_];

	protons el(np, Np, XY_distrib, vector2(0,0));

	particles = { el };
	
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


