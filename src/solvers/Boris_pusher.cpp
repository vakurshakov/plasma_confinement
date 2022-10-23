#include "Boris_pusher.hpp"

#include "src/pch.h"
#include "../particles/particle/particle.hpp"
#include "../vectors/vector3_field.hpp"
#include "../constants.h"


void Boris_interpolation::process(const vector2& r0, vector3& local_E, vector3& local_B) const
{
	const int nearest_edge_to_rx = int(round(r0.x()/dx));
	const int nearest_edge_to_ry = int(round(r0.y()/dy));

	vector2 shape[2];
	enum SHAPE {
		noshift = 0, //	shape[noshift] -- shape((i, j) - r)
		shifted = 1  //	shape[shifted] -- shape((i + 1./2, j + 1./2) - r)
	};

	for(int ny = nearest_edge_to_ry-charge_cloud_; ny <= nearest_edge_to_ry+charge_cloud_; ++ny) {
		shape[noshift].y() = shape_at_( ny*dy - r0.y(), dy);
		shape[shifted].y() = shape_at_((ny + 0.5)*dy - r0.y(), dy);
		
		for(int nx = nearest_edge_to_rx-charge_cloud_; nx <= nearest_edge_to_rx+charge_cloud_; ++nx) {
			shape[noshift].x() = shape_at_( nx*dx - r0.x(), dx);
			shape[shifted].x() = shape_at_((nx + 0.5)*dx - r0.x(), dx);
			
			local_E.x() += E_.x(ny,nx)*( shape[noshift].y() * shape[shifted].x() );
			local_E.y() += E_.y(ny,nx)*( shape[shifted].y() * shape[noshift].x() );
			local_E.z() += E_.z(ny,nx)*( shape[shifted].y() * shape[shifted].x() );
			
			local_B.x() += B_.x(ny,nx)*( shape[noshift].y() * shape[shifted].x() );
			local_B.y() += B_.y(ny,nx)*( shape[shifted].y() * shape[noshift].x() );
			local_B.z() += B_.z(ny,nx)*( shape[shifted].y() * shape[shifted].x() );
		}
	}
}


void Boris_pusher::process(Particle& particle, const vector3& local_E, const vector3& local_B) const
{
	// getting a usefull variabels from %parameters and %point
	const double q0 = 0.5 * dt * particle.q();
	const double m  = particle.m(); 
	vector2& r = particle.point.r;
	vector3& p = particle.point.p;

	// Realization of a Boris pusher

	const vector3 w = p + local_E * q0;
	
	double energy = sqrt(m * m + w.dot(w));
	
	const vector3 h = local_B * q0 / energy;
	
	const vector3 s = h * 2. / (1. + h.dot(h));
	
	p = local_E * q0 + w * (1. - h.dot(s)) + w.cross(s) + h * (s.dot(w));
	
	energy = sqrt(m * m + p.dot(p));
	
	r += p.squeeze(Axes::XY) * dt / energy;
}