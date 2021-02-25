#include "./particles_manager.hpp"


enum SOLV { PUSHER, DECOMP };
enum CONF { BOUND, n0_, ni_, NP, XY_ };

void Particles_manager::initialisation(vector<string> solvers,
	vector<string> configuration) {
	
	if ( solvers[PUSHER] == "Boris_pusher" ) {
		Particle_push_ = Boris_pusher;
	}

	if (solvers[DECOMP] == "Esirkepov_density_decomposition" ) {
		Density_decomposition_ = Esirkepov_density_decomposition;
	}


	string boundaries = configuration[BOUND];

	if ( boundaries == "reflective" ) {
		x_boundary = reflective_Xboundary;
		y_boundary = reflective_Yboundary;
	}
	else if ( boundaries == "periodic" ) {
		x_boundary = periodic_Xboundary;
		y_boundary = periodic_Yboundary;
	}
	else if ( boundaries == "rh_pv" ) {
		x_boundary = reflective_Xboundary;
		y_boundary = periodic_Yboundary;
	}
	else if ( boundaries == "rv_ph" ) {
		x_boundary = periodic_Xboundary;
		y_boundary = reflective_Yboundary;
	}

	// TODO: сделать перебор по сортам, которые так быстренько инициализируются
	//		for (int s = SORT; s < NUM_OF_SORTS; ++s) {
	//		if (sort[s] == "protons") {

	double n0 = stod(configuration[n0_]);
	double ni = stod(configuration[ni_]);
	double Np = stod(configuration[NP]);
	string XY_distrib = configuration[XY_];

	protons pr_(ni);
	electrons el_(n0, Np, XY_distrib, vector2(0,0));

	particles = { pr_, el_ };
}

void Particles_manager::Particle_push(const class_particles& SORT, particle& PARTICLE,
	const up_v3f& E, const up_v3f& B) {
	Particle_push_(SORT, PARTICLE, *E, *B);
}

void Particles_manager::Density_decomposition(const class_particles& SORT, const particle& PARTICLE,
	const vector2& r0, up_v3f& j) {
	Density_decomposition_(SORT, PARTICLE, r0, *j);
}

void Particles_manager::Boundaries_processing(particle& PARTICLE, double size_x, double size_y) {
	x_boundary(PARTICLE, size_x);
	y_boundary(PARTICLE, size_y);
}
