#ifndef VECTOR_CLASSES_H
#define VECTOR_CLASSES_H
	#include "../vectors/vector_classes.hpp"
#endif

#ifndef VECTOR3_FIELD_H
#define VECTOR3_FIELD_H
	#include "../vectors/vector3_field.hpp"
#endif
 
#ifndef CONSTANTS_H
#define CONSTANTS_H
	#include "../constants.h"
#endif

#include <vector>
#include <string>
#include <ctime>
#include <cmath>


class particle {
public:
	particle(): m_r(), m_p() {};
	particle(vector2& r, vector3& p): m_r(r), m_p(p) {};
	
	vector2& r() { return m_r; };
	double& x() { return m_r.x(); };
	double& y() { return m_r.y(); };
	
	vector2 r() const { return m_r; };
	double x() const { return m_r.x(); };
	double y() const { return m_r.y(); };

	vector3& p() { return m_p; };
	double& px() { return m_p.x(); };
	double& py() { return m_p.y(); };
	double& pz() { return m_p.z(); };
	
	vector3 p() const { return m_p; };
	double px() const { return m_p.x(); };
	double py() const { return m_p.y(); };
	double pz() const { return m_p.z(); };

private:
	vector2 m_r;
	vector3 m_p;
};


void reflective_Xboundaries_for(particle& particle_, double SIZE_X);
void reflective_Yboundaries_for(particle& particle_, double SIZE_Y);
void periodic_Xboundaries_for(particle& particle_, double SIZE_X);
void periodic_Yboundaries_for(particle& particle_, double SIZE_Y);

double second_order_spline(double x, double grid_mesh);
double third_order_spline(double x, double grid_mesh);
double fourth_order_spline(double x, double grid_mesh);
double fifth_order_spline(double x, double grid_mesh);

double frand();

class class_particles {
public:
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
	protons(double n, double Np, std::string XY_distrib, vector2 p0)
	: class_particles(+e, mpr, n, Np, second_order_spline, spline_width, XY_distrib, p0) {};
	protons(double n, double Np, std::string XY_distrib, vector3 p0)
	: class_particles(+e, mpr, n, Np, second_order_spline, spline_width, XY_distrib, p0) {};
	protons(double n, double Np, std::string XY_distrib, std::string P_distrib)
	: class_particles(+e, mpr, n, Np, second_order_spline, spline_width, XY_distrib, P_distrib) {};

};
