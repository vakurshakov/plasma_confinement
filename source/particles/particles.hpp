#define PARTICLES_H

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

#include <iostream>
#include <vector>
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

class sort_of_particles {
public:
	sort_of_particles(double q, double m, double n, double Np,
					 double (*form_factor)(double, double),
					 int charge_cloud) {
		m_q = q;
		m_m = m;
		m_n = n;
		m_Np = Np;
		m_form_factor = form_factor;
		m_charge_cloud = charge_cloud;
	}

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


class electrons : public sort_of_particles {
public:
	electrons(double n, double Np, vector3& p0) : sort_of_particles(-e, me, n, Np,
		third_order_spline, spline_width) {

	srand(time(NULL));

	int err = 0;
	for (int i = 0; i < (int)Np*SIZE_X*SIZE_Y + err; ++i) {

		double x = frand()*SIZE_X*dx;  
		double y = frand()*SIZE_Y*dy;  
		
		double px = p0.x() + sin(2.*M_PI*frand())*sqrt(-2.*(Tx*me/mec2)*log(frand())); 
		double py = p0.y() + sin(2.*M_PI*frand())*sqrt(-2.*(Ty*me/mec2)*log(frand()));
		double pz = p0.z(); // sin(2.*M_PI*frand())*sqrt(-2.*(Tz*me/mec2)*log(frand()));

		if (std::isinf(px) | std::isinf(py) | std::isinf(pz)) { 
			//std::cout << "\t\tpx, py or pz is inf!" << std::endl;
			++err;
			continue;
		}
		
		vector2 r(x, y);
		vector3 p(px, py, pz);		
		
		particle temp_(r, p);
		m_particles.push_back(temp_);		
	}
	}
};
