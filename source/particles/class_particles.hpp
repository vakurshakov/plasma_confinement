#include "../vectors/vector3_field.hpp" 
#include "../constants.h"
#include "./particle.hpp"

#include <vector>
#include <string>
#include <ctime>
#include <cmath>

#ifndef CLASS_PARTICLES_H
#define CLASS_PARTICLES_H

//#################################################################################################

double frand();

class class_particles {
public:
	class_particles(double q, double m, double n);

	class_particles(double q, double m, double n, double Np,
					double (*form_factor)(double, double), int charge_cloud,
					std::string XY_distrib, vector2 p0);
	
	class_particles(double q, double m, double n, double Np,
					double (*form_factor)(double, double), int charge_cloud,
					std::string XY_distrib, vector3 p0);

	class_particles(double q, double m, double n, double Np,
					double (*form_factor)(double, double), int charge_cloud,
					std::string XY_distrib, std::string P_distrib);


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
	double m_q;
	double m_m;
	double m_n;
	double m_Np;
	double (*m_form_factor)(double, double);
	int m_charge_cloud;
	std::vector<particle> m_particles;
};


class electrons : public class_particles {
public:
	electrons(double n, double Np, std::string XY_distrib, vector2 p0)
	: class_particles(-e, me, n, Np, second_order_spline, spline_width, XY_distrib, p0) {};

	electrons(double n, double Np, std::string XY_distrib, vector3 p0)
	: class_particles(-e, me, n, Np, second_order_spline, spline_width, XY_distrib, p0) {};

	electrons(double n, double Np, std::string XY_distrib, std::string P_distrib)
	: class_particles(-e, me, n, Np, second_order_spline, spline_width, XY_distrib, P_distrib) {};

};


class protons : public class_particles {
public: 
	protons(double n)
	: class_particles(+e, mpr, n) {};
	
	protons(double n, double Np, std::string XY_distrib, vector2 p0)
	: class_particles(+e, mpr, n, Np, second_order_spline, spline_width, XY_distrib, p0) {};
	
	protons(double n, double Np, std::string XY_distrib, vector3 p0)
	: class_particles(+e, mpr, n, Np, second_order_spline, spline_width, XY_distrib, p0) {};
	
	protons(double n, double Np, std::string XY_distrib, std::string P_distrib)
	: class_particles(+e, mpr, n, Np, second_order_spline, spline_width, XY_distrib, P_distrib) {};
	
};

//#################################################################################################

#endif
