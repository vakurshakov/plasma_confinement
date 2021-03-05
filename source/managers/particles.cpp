#include "./particles.hpp"
#include <string>
#include <iostream>


enum SOLV { PUSHER, DECOMP };
enum CONF { BOUND, n_, Np_, XY_, P_type_, P_};
//enum PD_DESC {  };

void Particles::initialize(vector<string> solvers, vector<string> configuration,
	string test_name, vector<string> diagnostics_description)
{
	// initialize of solvers
	if ( solvers[PUSHER] == "Boris_pusher" ) {
		particle_push_ = Boris_pusher;
	}

	if (solvers[DECOMP] == "Esirkepov_density_decomposition" ) {
		density_decomposition_ = Esirkepov_density_decomposition;
	}

	// initialize of configuration
	string boundaries = configuration[BOUND];

	if ( boundaries == "reflective" ) {
		x_boundary_ = reflective_Xboundary;
		y_boundary_ = reflective_Yboundary;
	}
	else if ( boundaries == "periodic" ) {
		x_boundary_ = periodic_Xboundary;
		y_boundary_ = periodic_Yboundary;
	}
	else if ( boundaries == "rh_pv" ) {
		x_boundary_ = reflective_Xboundary;
		y_boundary_ = periodic_Yboundary;
	}
	else if ( boundaries == "rv_ph" ) {
		x_boundary_ = periodic_Xboundary;
		y_boundary_ = reflective_Yboundary;
	}

	// TODO : частицы обёртки? Only Species_decription

	double n 			= stod(configuration[CONF::n_]);
	double Np 			= stod(configuration[CONF::Np_]);
	string XY_distrib 	= 	   configuration[CONF::XY_];
	string P_type 		= 	   configuration[CONF::P_type_];
	string P_distrib 	= 	   configuration[CONF::P_];

	n_ = n;

	if (P_type == "vector2") {
		// P_distrib = "0.0 0.0"

		int divider = P_distrib.find(' ');
		int end = P_distrib.size();

		double px = stod(P_distrib.substr(0, divider));
		double py = stod(P_distrib.substr(divider, end));

		vector2 p0(px, py);

		load_p02d_particles(*this, Np, XY_distrib, p0);
	}
	else if (P_type == "vector3") {
		// P_distrib = "0.0, 0.0, 0.0"

		int divider_1 = P_distrib.find(' ');
		int divider_2 = P_distrib.find(' ', divider_1);
		int end = P_distrib.size();


		double px = stod(P_distrib.substr(0, divider_1));
		double py = stod(P_distrib.substr(divider_1, divider_2));
		double pz = stod(P_distrib.substr(divider_2, end));

		vector3 p0(px, py, pz);
	
		load_p03d_particles(*this, Np, XY_distrib, p0);
	}
	else if (P_type == "string") {
		load_chosen_distribution(*this, Np, XY_distrib, P_distrib);
	}


	//initialize of diagnostics
	for (auto& diagnostic : diagnostics_description) {
		if ( diagnostic == "energy" ) {
			diagnostics_.emplace_back(make_unique<particles_energy>(test_name + "/" + diagnostic));
		}
		else if ( diagnostic == "phase_diagram" ) {
			diagnostics_.emplace_back(make_unique<phase_diagram>(test_name + "/" + diagnostic));
		}
		else {
			diagnostics_.emplace_back(nullptr);
		}
 	}

	diagnostics_.shrink_to_fit();
}


void Particles::particle_push(int i, const v3f& E, const v3f& B)
{
	particle_push_(*this, particles_[i], E, B);
} 


void Particles::density_decomposition(int i, const vector2& r0, v3f& j)
{
	density_decomposition_(*this, particles_[i], r0, j);
}


void Particles::boundaries_processing(int i, double size_x, double size_y)
{
	x_boundary_(particles_[i], size_x);
	y_boundary_(particles_[i], size_y);
}


void Particles::diagnose()
{
	if (diagnostics_[0] != nullptr) {
		for (auto& diagnostic : diagnostics_) {
			(*diagnostic).diagnose(*this);
		}
	} 
}
