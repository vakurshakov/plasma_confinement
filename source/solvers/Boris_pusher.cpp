#ifndef VECTOR3_FIELD_H
#define VECTOR3_FIELD_H
	#include "../vectors/vector3_field.hpp"
#endif

#ifndef PARTICLES_H
#define PARTICLES_H
	#include "../particles/particles.hpp"
#endif

#ifndef CONSTANTS_H
#define CONSTANTS_H
	#include "../constants.h"
#endif

#include <cmath>

enum SHAPE {
	noshift = 0,
	shifted = 1
};

void Boris_pusher(const sort_of_particles& SORT,
				  particle& PARTICLE,
				  const vector3_field& E,
				  const vector3_field& B)
{
	// getting a usefull variabels from %SORT and %PARTICLE
	vector2 r = PARTICLE.r();
	vector3 p = PARTICLE.p();

	double  q = SORT.q(); 
	double  m = SORT.m();
	double (*shape_at)(double, double) = SORT.form_factor();
	int charge_cloud = SORT.charge_cloud();

	int nearest_edge_to_rx = int(roundf(r.x()/dx));
	int nearest_edge_to_ry = int(roundf(r.y()/dy));

	vector3 local_E, local_B;
	vector2 shape[2];
	//	shape[noshift] -- shape((i, j) - r)
	//	shape[shifted] -- shape((i + 1./2, j + 1./2) - r)
		
	for(int y = nearest_edge_to_ry-charge_cloud; y <= nearest_edge_to_ry+charge_cloud; ++y) {
		shape[noshift].y() = shape_at( y*dy - r.y(), dy);
		shape[shifted].y() = shape_at((y + 0.5)*dy - r.y(), dy);
		
		for(int x = nearest_edge_to_rx-charge_cloud; x <= nearest_edge_to_rx+charge_cloud; ++x) {
			shape[noshift].x() = shape_at( x*dx - r.x(), dx);
			shape[shifted].x() = shape_at((x + 0.5)*dx - r.x(), dx);
			
			local_E.x() += E.x(y,x)*( shape[shifted].x() * shape[noshift].y() ); // noshift, shifted
			local_E.y() += E.y(y,x)*( shape[noshift].x() * shape[shifted].y() ); // shifted, noshift
			local_E.z() += E.z(y,x)*( shape[shifted].x() * shape[shifted].y() ); // shifted, shifted
			
			local_B.x() += B.x(y,x)*( shape[shifted].x() * shape[noshift].y() ); // shifted, noshift
			local_B.y() += B.y(y,x)*( shape[noshift].x() * shape[shifted].y() ); // noshift, shifted
			local_B.z() += B.z(y,x)*( shape[shifted].x() * shape[shifted].y() ); // noshift, noshift
		}
	}

	vector3 p_minus, h, s;
	double energy;

	p_minus = p + local_E*0.5*q*dt;
	energy = sqrt(m*m + p_minus.dot(p_minus));
	h = local_B*0.5*q*dt/energy;
	s = h*2./(1. + h.dot(h));
	
	p = local_E*0.5*q*dt + p_minus*(1. - h.dot(s)) + p_minus.cross(s) + h*(s.dot(p_minus));
	
	energy = sqrt(m*m + p.dot(p));
	r += p.squeeze(Axes::XY)/energy*dt;

	PARTICLE.r() = r;
	PARTICLE.p() = p;			
}