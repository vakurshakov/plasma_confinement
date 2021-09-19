#ifndef SOLVERS_H
#define SOLVERS_H

//#################################################################################################

#include "../vectors/vector3_field.hpp"
#include "../particle/particle_parameters.hpp"
#include "../particle/point.hpp"

void Boris_pusher(const Particle_parameters&, Point&,
				  const vector3_field& E, const vector3_field& B);

void Esirkepov_density_decomposition(const Particle_parameters&,
									 const Point&,
									 const vector2& r0,
									 vector3_field& J);

void FDTD_2D(vector3_field& E, vector3_field& B, vector3_field& j);

//#################################################################################################

#endif