#include "../vectors/vector3_field.hpp"
#include "../particles/class_particles.hpp"


#ifndef SOLVERS_H
#define SOLVERS_H

//#################################################################################################

void Boris_pusher(const class_particles& SORT, particle& PARTICLE,
				  const vector3_field& E, const vector3_field& B);

void Esirkepov_density_decomposition(const class_particles& SORT,
									 const particle& PARTICLE,
									 const vector2& r0,
									 vector3_field& J);

void FDTD_2D(vector3_field& E, vector3_field& B, vector3_field& j);

//#################################################################################################

#endif