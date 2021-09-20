#include "./Boris_pusher.hpp"

#include <cmath>
#include <functional>

#include "../particle/particle_parameters.hpp"
#include "../particle/point.hpp"
#include "../vectors/vector3_field.hpp"
#include "../constants.h"


enum SHAPE {
	noshift = 0,
	shifted = 1
};


void Boris_pusher::interpolate(const v3f& E, const v3f& B,
	const Particle_parameters& parameters, const vector2& r0)
{
	const std::function<double(double, double)> shape_at = parameters.form_factor();
	const int charge_cloud = parameters.charge_cloud();

	const int nearest_edge_to_rx = static_cast<int>(roundf(r0.x/dx));
	const int nearest_edge_to_ry = static_cast<int>(roundf(r0.y/dy));

	vector2 shape[2];
	//	shape[noshift] -- shape((i, j) - r)
	//	shape[shifted] -- shape((i + 1./2, j + 1./2) - r)
		
	for(int ny = nearest_edge_to_ry-charge_cloud; ny <= nearest_edge_to_ry+charge_cloud; ++ny) {
		shape[noshift].y = shape_at( ny*dy - r0.y, dy);
		shape[shifted].y = shape_at((ny + 0.5)*dy - r0.y, dy);
		
		for(int nx = nearest_edge_to_rx-charge_cloud; nx <= nearest_edge_to_rx+charge_cloud; ++nx) {
			shape[noshift].x = shape_at( nx*dx - r0.x, dx);
			shape[shifted].x = shape_at((nx + 0.5)*dx - r0.x, dx);
			
			local_E.x += E.x(ny,nx)*( shape[shifted].x * shape[noshift].y );
			local_E.y += E.y(ny,nx)*( shape[noshift].x * shape[shifted].y );
			local_E.z += E.z(ny,nx)*( shape[shifted].x * shape[shifted].y );
			
			local_B.x += B.x(ny,nx)*( shape[shifted].x * shape[noshift].y );
			local_B.y += B.y(ny,nx)*( shape[noshift].x * shape[shifted].y );
			local_B.z += B.z(ny,nx)*( shape[shifted].x * shape[shifted].y );
		}
	}
}

void Boris_pusher::push(const Particle_parameters& parameters, Point* point)
{
	// getting a usefull variabels from %parameters and %point
	r_ = point->r();
	p_ = point->p();
	const double  q = parameters.q(); 
	const double  m = parameters.m();

	const vector3 p_minus = p_ + local_E*0.5*q*dt;
	
	double energy = sqrt(m*m + p_minus.dot(p_minus));
	
	const vector3 h = local_B*0.5*q*dt/energy;
	
	const vector3 s = h*2./(1. + h.dot(h));
	
	p_ = local_E*0.5*q*dt + p_minus*(1. - h.dot(s)) + p_minus.cross(s) + h*(s.dot(p_minus));
	
	energy = sqrt(m*m + p_.dot(p_));
	
	r_ += p_.squeeze(Axes::XY)/energy*dt;

	point->r() = r_;
	point->p() = p_;		
}