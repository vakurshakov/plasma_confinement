#include "../vectors/vector3_field.hpp"
#include "../particles/species_description.hpp"
#include "../managers/fields_additionals/magnetic_mirror.hpp"
#include "../constants.h"

#include <functional>
#include <cmath>


enum SHAPE {
	noshift = 0,
	shifted = 1
};

void Boris_pusher(const Species_description& sort,
				  Particle& particle,
				  const vector3_field& E,
				  const vector3_field& B)
{
	// getting a usefull variabels from %sort and %particle
	vector2 r = particle.r();
	vector3 p = particle.p();

	double  q = sort.q(); 
	double  m = sort.m();
	function<double(double, double)> shape_at = sort.form_factor();
	int charge_cloud = sort.charge_cloud();

	int nearest_edge_to_rx = int(roundf(r.x/dx));
	int nearest_edge_to_ry = int(roundf(r.y/dy));

	vector3 local_E, local_B;
	vector2 shape[2];
	//	shape[noshift] -- shape((i, j) - r)
	//	shape[shifted] -- shape((i + 1./2, j + 1./2) - r)
		
	for(int y = nearest_edge_to_ry-charge_cloud; y <= nearest_edge_to_ry+charge_cloud; ++y) {
		shape[noshift].y = shape_at( y*dy - r.y, dy);
		shape[shifted].y = shape_at((y + 0.5)*dy - r.y, dy);
		
		for(int x = nearest_edge_to_rx-charge_cloud; x <= nearest_edge_to_rx+charge_cloud; ++x) {
			shape[noshift].x = shape_at( x*dx - r.x, dx);
			shape[shifted].x = shape_at((x + 0.5)*dx - r.x, dx);
			
			local_E.x += E.x(y,x)*( shape[shifted].x * shape[noshift].y ); // noshift, shifted
			local_E.y += E.y(y,x)*( shape[noshift].x * shape[shifted].y ); // shifted, noshift
			local_E.z += E.z(y,x)*( shape[shifted].x * shape[shifted].y ); // shifted, shifted
			
			local_B.x += B.x(y,x)*( shape[shifted].x * shape[noshift].y ); // shifted, noshift
			local_B.y += B.y(y,x)*( shape[noshift].x * shape[shifted].y ); // noshift, shifted
			local_B.z += B.z(y,x)*( shape[shifted].x * shape[shifted].y ); // noshift, noshift
		}
	}

	//	добавляем внешнее, аналитически вычисленное, маг. поле
	local_B += ( mirror1.return_field(r) + mirror2.return_field(r) );

	vector3 p_minus, h, s;
	double energy;

	p_minus = p + local_E*0.5*q*dt;
	energy = sqrt(m*m + p_minus.dot(p_minus));
	h = local_B*0.5*q*dt/energy;
	s = h*2./(1. + h.dot(h));
	
	p = local_E*0.5*q*dt + p_minus*(1. - h.dot(s)) + p_minus.cross(s) + h*(s.dot(p_minus));
	
	energy = sqrt(m*m + p.dot(p));
	r += p.squeeze(Axes::XY)/energy*dt;

	particle.r() = r;
	particle.p() = p;			
}