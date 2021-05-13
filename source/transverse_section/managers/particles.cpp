#include "./particles.hpp"
#include <string>
#include <iostream>


enum SOLV { PUSHER, DECOMP };
enum CONF { BOUND, ns_, Nps_, XY_, P_type_, P_};
//enum PD_DESC {  };

void Particles::initialize(string p_name, vector<string> solvers, vector<string> configuration,
	string dir_name, vector<string> diagnostics_description)
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

	if ( boundaries == "rx_ry" ) {
		x_boundary_ = reflective_Xboundary;
		y_boundary_ = reflective_Yboundary;
	}
	else if ( boundaries == "px_py" ) {
		x_boundary_ = periodic_Xboundary;
		y_boundary_ = periodic_Yboundary;
	}
	else if ( boundaries == "rx_py" ) {
		x_boundary_ = reflective_Xboundary;
		y_boundary_ = periodic_Yboundary;
	}
	else if ( boundaries == "px_ry" ) {
		x_boundary_ = periodic_Xboundary;
		y_boundary_ = reflective_Yboundary;
	}

	// TODO : частицы обёртки? Only Species_decription

	double n 			= stod(configuration[CONF::ns_]);
	int Np 				= stoi(configuration[CONF::Nps_]);
	string XY_distrib 	= 	   configuration[CONF::XY_];
	string P_type 		= 	   configuration[CONF::P_type_];
	string P_module 	= 	   configuration[CONF::P_];

	n_ = n;
	Np_ = Np;

	// TODO : сделать направленную скорость для КАЖДОЙ частицы, 
	//	а не для всего сорта сразу. Т.о. load_%_particle должен
	//	работать на одну частицу: давать ей координаты и импульсы.
	double p0 = stod(P_module);

	if (P_type == "vector2") {
		// P_module = "abs(p0>)"
		load_p02d_particles(*this, XY_distrib, p0);
	}
	else if (P_type == "vector3") {
		// P_module = "abs(p0>)"
		load_p03d_particles(*this, XY_distrib, p0);
	}

	if (particles_are_diagnosed) {
		//initialize of diagnostics
		for (auto& diagnostic : diagnostics_description) {
			if ( diagnostic == "energy" ) {
				diagnostics_.emplace_back(make_unique<particles_energy>(dir_name + '/' + p_name + '/' + diagnostic));
			}
			else if ( diagnostic == "diagram_vx_on_y" ) {
				diagnostics_.emplace_back(make_unique<diagram_vx_on_y>(dir_name + '/' + p_name + '/' + diagnostic,
					-1., 1., 0.01, 0, SIZE_Y));
			}
			else if ( diagnostic == "density" ) {
				diagnostics_.emplace_back(make_unique<density>(dir_name + '/' + p_name + '/' + diagnostic));
			}
	 	}
		diagnostics_.shrink_to_fit();
	}

	std::cout << "\t" << particles_.size() << "\tparticles have been loaded" << std::endl;
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
	for (auto& diagnostic : diagnostics_) {
		(*diagnostic).diagnose(*this);
	} 
}
