#ifndef VECTOR_CLASSES_H
#define VECTOR_CLASSES_H
	#include "../vectors/vector_classes.hpp"
#endif

#ifndef VECTOR3_FIELD_H
#define VECTOR3_FIELD_H
	#include "../vectors/vector3_field.hpp"
#endif

#include <vector>

class particle {
public:
	particle(): m_r(), m_p() {};
	particle(vector2& r, vector3& p): m_r(r), m_p(p) {};
	

	vector2& r() { return m_r; };
	vector3& p() { return m_p; };
	vector2 r() const { return m_r; };
	vector3 p() const { return m_p; };

private:
	vector2 m_r;
	vector3 m_p;
};

class sort_of_particles {
public:
	sort_of_particles(double q, double m, double Np,
					  double (*form_factor)(double, double),
					  int charge_cloud) {
		m_q = q;
		m_m = m;
		m_Np = Np;
		m_form_factor = form_factor;
		m_charge_cloud = charge_cloud;
	}

	double q() const { return m_q; };
	double m() const { return m_m; };
	double Np() const { return m_Np; };
	auto form_factor() const { return m_form_factor; };
	int charge_cloud() const { return m_charge_cloud; };
	int amount() const { return m_particles.size(); };

	auto& particles() { return m_particles; };
	particle& particles(int i) { return m_particles[i]; };

private:
	double m_q;
	double m_m;
	double m_Np;
	double (*m_form_factor)(double, double);
	int m_charge_cloud;
	std::vector<particle> m_particles;
};

void reflective_Xboundaries_for(particle& particle_, double SIZE_X);
void reflective_Yboundaries_for(particle& particle_, double SIZE_Y);
void periodic_Xboundaries_for(particle& particle_, double SIZE_X);
void periodic_Yboundaries_for(particle& particle_, double SIZE_Y);

double second_order_spline(double x, double grid_mesh);
double third_order_spline(double x, double grid_mesh);
double fourth_order_spline(double x, double grid_mesh);
double fifth_order_spline(double x, double grid_mesh);