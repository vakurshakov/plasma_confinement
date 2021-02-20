#ifndef VECTOR_CLASSES_H
#define VECTOR_CLASSES_H
	#include "../vectors/vector_classes.hpp"
#endif

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
#include <iostream>

void add_circular_current(vector3_field& j, class_particles& sort, double v_inj, double Bz)
{
	double q = sort.q();
	double m = sort.m();
	double n = sort.n();
	double Np = sort.Np();

	double gamma = 1./sqrt(1 - v_inj*v_inj);
	double r_larm = gamma*m*v_inj/(q*Bz);


	double dphi;	// NOTE: как аккуратно заполнять массив так, ч
	double ds = 1;		// тобы в одно место дважды не попадать?

	double x, y, vx, vy;

	
	for (double r = r_larm - ds; r < r_larm + ds; r += ds) {
		dphi = dy/(r*dx);
		for (double phi = 0; phi < 2*M_PI; phi += dphi) {
			
			x = SIZE_X/2 + r*cos(phi);
			y = SIZE_Y/2 + r*sin(phi);

			vx = + v_inj*sin(phi);
			vy = - v_inj*cos(phi);

			j.x(y,x) = q*n*vx/Np;
			j.y(y,x) = q*n*vy/Np;
	}
	}
}