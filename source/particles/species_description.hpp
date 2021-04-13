#ifndef SPECIES_DESCRIPTION_H
#define SPECIES_DESCRIPTION_H

//#################################################################################################

#include "../vectors/vector3_field.hpp" 
#include "./particle.hpp"
#include "../constants.h"


#include <functional>
#include <vector>
#include <string>
#include <ctime>
#include <cmath>

using namespace std;


double frand();

class Species_description {
public:
	Species_description() = default;

	Species_description(double q, double m,
		function<double(double, double)> form_factor, int charge_cloud)
	: q_(q), m_(m), form_factor_(form_factor), charge_cloud_(charge_cloud) {};

	Species_description(double q, double m, double n,
		function<double(double, double)> form_factor, int charge_cloud)
	: q_(q), m_(m), n_(n), form_factor_(form_factor), charge_cloud_(charge_cloud) {};

	// getters
	double q() const { return q_; };
	double m() const { return m_; };
	double n() const { return n_; };
	double Np() const { return Np_; };
	auto form_factor() const { return form_factor_; };
	int charge_cloud() const { return charge_cloud_; };
	int amount() const { return particles_.size(); };
	
	Particle& element(int i) { return particles_[i]; };
	Particle element(int i) const { return particles_[i]; };

	friend void load_p02d_particles(Species_description& sort, double Np,
		string XY_distrib, double cX, double cY, double Xm, double Ym, const vector2& p0);
	friend void load_p03d_particles(Species_description& sort, double Np,
		string XY_distrib, double cX, double cY, double Xm, double Ym, const vector3& p0);
	friend void load_chosen_distribution(Species_description& sort, double Np,
		string XY_distrib, double cX, double cY, double Xm, double Ym, string P_distrib);

protected:
	// spescies parameters
	double q_;
	double m_;
	double n_;
	double Np_;
	function<double(double, double)> form_factor_;
	int charge_cloud_;
	vector<Particle> particles_;
};


double second_order_spline(double x, double grid_mesh);
double third_order_spline(double x, double grid_mesh);
double fourth_order_spline(double x, double grid_mesh);
double fifth_order_spline(double x, double grid_mesh);

//#################################################################################################

#endif
