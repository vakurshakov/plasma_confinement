#ifndef VECTOR_CLASSES_H
#define VECTOR_CLASSES_H
	#include "../vectors/vector_classes.hpp"
#endif

#ifndef VECTOR3_FIELD_H
#define VECTOR3_FIELD_H
	#include "../vectors/vector3_field.hpp"
#endif


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


void reflective_Xboundary(particle& particle_, double SIZE_X);
void reflective_Yboundary(particle& particle_, double SIZE_Y);
void periodic_Xboundary(particle& particle_, double SIZE_X);
void periodic_Yboundary(particle& particle_, double SIZE_Y);

double second_order_spline(double x, double grid_mesh);
double third_order_spline(double x, double grid_mesh);
double fourth_order_spline(double x, double grid_mesh);
double fifth_order_spline(double x, double grid_mesh);
