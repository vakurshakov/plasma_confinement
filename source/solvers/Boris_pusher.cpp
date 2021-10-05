#include "Boris_pusher.hpp"

#include <cmath>
#include <functional>

#include "../particles/particle/particle_parameters.hpp"
#include "../particles/particle/point.hpp"
#include "../vectors/vector3_field.hpp"
#include "../constants.h"


void Boris_pusher::interpolate(const v3f& E, const v3f& B,
	const Particle_parameters& parameters, const vector2& r0)
{
	const std::function<double(double, double)> shape_at = parameters.form_factor();
	const int charge_cloud = parameters.charge_cloud();

	const int nearest_edge_to_rx = int(roundf(r0.x/dx));
	const int nearest_edge_to_ry = int(roundf(r0.y/dy));

	vector2 shape[2];
	enum SHAPE {
		noshift = 0, //	shape[noshift] -- shape((i, j) - r)
		shifted = 1  //	shape[shifted] -- shape((i + 1./2, j + 1./2) - r)
	};
		
	local_E = {0, 0, 0};
	local_B = {0, 0, 0};
	for(int ny = nearest_edge_to_ry-charge_cloud; ny <= nearest_edge_to_ry+charge_cloud; ++ny) {
		shape[noshift].y = shape_at( ny*dy - r0.y, dy);
		shape[shifted].y = shape_at((ny + 0.5)*dy - r0.y, dy);
		
		for(int nx = nearest_edge_to_rx-charge_cloud; nx <= nearest_edge_to_rx+charge_cloud; ++nx) {
			shape[noshift].x = shape_at( nx*dx - r0.x, dx);
			shape[shifted].x = shape_at((nx + 0.5)*dx - r0.x, dx);
			
			local_E.x += E.x(ny,nx)*( shape[noshift].y * shape[shifted].x );
			local_E.y += E.y(ny,nx)*( shape[shifted].y * shape[noshift].x );
			local_E.z += E.z(ny,nx)*( shape[shifted].y * shape[shifted].x );
			
			local_B.x += B.x(ny,nx)*(  shape[noshift].y * shape[shifted].x );
			local_B.y += B.y(ny,nx)*(  shape[shifted].y * shape[noshift].x );
			local_B.z += B.z(ny,nx)*(  shape[shifted].y * shape[shifted].x );
		}
	}
}


void Boris_pusher::push(const Particle_parameters& parameters, Point& point)
{
	// getting a usefull variabels from %parameters and %point
	vector2 r_ = point.r();
	vector3 p_ = point.p();
	const double  q = parameters.q(); 
	const double  m = parameters.m();

	// Realization of a Boris pusher
	const double q0 = 0.5*q*dt;

	const vector3 w = p_ + local_E*q0;
	
	double energy = sqrt(m*m + w.dot(w));
	
	const vector3 h = local_B*q0/energy;
	
	const vector3 s = h*2./(1. + h.dot(h));
	
	p_ = local_E*q0 + w*(1. - h.dot(s)) + w.cross(s) + h*(s.dot(w));
	
	energy = sqrt(m*m + p_.dot(p_));
	
	r_ += p_.squeeze(Axes::XY)*dt/energy;

	point.r() = r_;
	point.p() = p_;		
}
