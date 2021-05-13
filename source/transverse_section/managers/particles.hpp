
#ifndef PARTICLES_H
#define PARTICLES_H

//#################################################################################################

#include "../solvers/solvers.hpp"
#include "../vectors/vector3_field.hpp"
#include "../particles/species_description.hpp"
#include "../diagnostics/diagnostics.hpp"
#include "../constants.h"

#include <functional>
#include <memory>


using namespace std;
using v3f = vector3_field;
using diagnostic_up = unique_ptr<Diagnostic>;

const int spline_width = 4;


class Particles : public Species_description {
public:
	Particles() = default;

	Particles(double q, double m)
	: Species_description(q, m, fourth_order_spline, spline_width) {};

	Particles(double q, double m, double n)
	: Species_description(q, m, n, fourth_order_spline, spline_width) {};

	void initialize(string name, vector<string> solvers, vector<string> configuration,
	string dir_name, vector<string> diagnostics);

	// solvers & diagnostics
	void particle_push(int i, const v3f& E, const v3f& B); 
	void density_decomposition(int i, const vector2& r0, v3f& j);
	void boundaries_processing(int i, double size_x, double size_y);
	void diagnose();

	void add_ion_current(int t);
	
protected:
	// vector of diagnostics for sort
	vector<diagnostic_up> diagnostics_; 

	// solvers for particles
	function<void(const Species_description&, Particle&, const v3f& E, const v3f& B)> particle_push_ = nullptr;
	function<void(const Species_description&, Particle&, const vector2& r0, v3f& j)> density_decomposition_ = nullptr;
	function<void(Particle&, double size_x, double size_y)> boundaries_processing_ = nullptr;

	function<void(Particle&, double)> x_boundary_ = nullptr;
	function<void(Particle&, double)> y_boundary_ = nullptr;
};

class Electrons : public Particles {
public:
	Electrons() : Particles(-e, me) {};
	
	Electrons(double n) : Particles(-e, me, n) {};
};


class Protons : public Particles {
public: 
	Protons() : Particles(+e, mpr) {};

	Protons(double n) : Particles(+e, mpr, n) {};

	//void add_ion_current(int t);
};

class Ions : public Particles {
public: 
	Ions() : Particles(+e, mi) {};

	Ions(double n) : Particles(+e, mi, n) {};

	void add_ion_current(int t);
};
//#################################################################################################

#endif
