
#ifndef PARTICLES_H
#define PARTICLES_H

//#################################################################################################

#include "../solvers/solvers.hpp"
#include "../vectors/vector3_field.hpp"
#include "../particles/species_description.hpp"
#include "../diagnostics/diagnostics.hpp"
#include "../constants.h"

#include <memory>
#include <functional>

using namespace std;
using v3f = vector3_field;
using diagnostic_up = unique_ptr<diagnostic>;


class Particles : public Species_description {
public:
	Particles() = default;

	// TODO: нужно как-то привинтить инициализацию строками, ведь нам Manager их суёт

	Particles(double q, double m, double n)
	: Species_description(q, m, n) {};
	
	Particles(double q, double m, double n, double Np,
		double (*form_factor)(double, double), int charge_cloud,
		string XY_distrib, vector2 p0)
	: Species_description(q, m, n, Np, form_factor, charge_cloud, XY_distrib, p0) {};
	
	Particles(double q, double m, double n, double Np,
		double (*form_factor)(double, double), int charge_cloud,
		string XY_distrib, vector3 p0)
	: Species_description(q, m, n, Np, form_factor, charge_cloud, XY_distrib, p0) {};

	Particles(double q, double m, double n, double Np, 
		double (*form_factor)(double, double), int charge_cloud,
		string XY_distrib, string P_distrib)
	: Species_description(q, m, n, Np, form_factor, charge_cloud, XY_distrib, P_distrib) {};

	void initialization();

	// solvers & diagnostics
	void particle_push(int i, const v3f& E, const v3f& B); 
	void density_decomposition(int i, const vector2& r0, v3f& j) const;
	void boundaries_processing(int i, double size_x, double size_y);
	void diagnose();


protected:
	// vector of diagnostics for sort
	vector<diagnostic_up> diagnostics; 

	// solvers for particles
	function<void(int i, const v3f& E, const v3f& B)> particle_push_;
	function<const void(int i, const vector2& r0, v3f& j)> density_decomposition_;
	function<void(int i, double size_x, double size_y)> boundaries_processing_;

	function<void(int i, double)> x_boundary;
	function<void(int i, double)> y_boundary;
};

class electrons : public Particles {
public:
	electrons(double n, double Np, string XY_distrib, vector2 p0)
	: Particles(-e, me, n, Np, second_order_spline, spline_width, XY_distrib, p0) {};

	electrons(double n, double Np, string XY_distrib, vector3 p0)
	: Particles(-e, me, n, Np, second_order_spline, spline_width, XY_distrib, p0) {};

	electrons(double n, double Np, string XY_distrib, string P_distrib)
	: Particles(-e, me, n, Np, second_order_spline, spline_width, XY_distrib, P_distrib) {};

};


class protons : public Particles {
public: 
	protons(double n)
	: Particles(+e, mpr, n) {};
	
	protons(double n, double Np, string XY_distrib, vector2 p0)
	: Particles(+e, mpr, n, Np, second_order_spline, spline_width, XY_distrib, p0) {};
	
	protons(double n, double Np, string XY_distrib, vector3 p0)
	: Particles(+e, mpr, n, Np, second_order_spline, spline_width, XY_distrib, p0) {};
	
	protons(double n, double Np, string XY_distrib, string P_distrib)
	: Particles(+e, mpr, n, Np, second_order_spline, spline_width, XY_distrib, P_distrib) {};
	
};

//#################################################################################################

#endif
