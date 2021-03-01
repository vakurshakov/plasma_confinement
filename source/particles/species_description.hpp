#ifndef SPECIES_DESCRIPTION_H
#define SPECIES_DESCRIPTION_H

//#################################################################################################

#include "../vectors/vector3_field.hpp" 
#include "../diagnostics/diagnostics.hpp"
#include "../solvers/solvers.hpp"
#include "./particle.hpp"
#include "../constants.h"


#include <functional>
#include <vector>
#include <string>
#include <ctime>
#include <cmath>

using namespace std;
using v3f = vector3_field;
using diagnostic_up = unique_ptr<diagnostic>;


double frand();

class Species_description {
public:
	Species_description() = default;

	Species_description(double q, double m, double n);
	
	Species_description(double q, double m, double n, double Np,
		double (*form_factor)(double, double), int charge_cloud,
		string XY_distrib, vector2 p0);
	
	Species_description(double q, double m, double n, double Np,
		double (*form_factor)(double, double), int charge_cloud,
		string XY_distrib, vector3 p0);

	Species_description(double q, double m, double n, double Np, 
		double (*form_factor)(double, double), int charge_cloud,
		string XY_distrib, string P_distrib);


	// getters
	double q() const { return m_q; };
	double m() const { return m_m; };
	double n() const { return m_n; };
	double Np() const { return m_Np; };
	auto form_factor() const { return m_form_factor; };
	int charge_cloud() const { return m_charge_cloud; };
	int amount() const { return m_particles.size(); };
	
	particle& element(int i) { return m_particles[i]; };
	particle element(int i) const { return m_particles[i]; };

protected:
	// spescies parameters
	double m_q;
	double m_m;
	double m_n;
	double m_Np;
	double (*m_form_factor)(double, double);
	int m_charge_cloud;
	vector<particle> m_particles;

};
//#################################################################################################

#endif
