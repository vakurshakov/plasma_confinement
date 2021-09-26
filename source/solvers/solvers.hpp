#ifndef SOLVERS_H
#define SOLVERS_H

//#################################################################################################

#include "../particles/particle/particle_parameters.hpp"
#include "../particles/particle/point.hpp"
#include "../vectors/vector_classes.hpp"
#include "../vectors/vector3_field.hpp"


void Esirkepov_density_decomposition(const Particle_parameters&,
									 const Point&,
									 const vector2& r0,
									 vector3_field* const J);

void FDTD_2D(vector3_field* const E, vector3_field* const B, vector3_field* const J);

//#################################################################################################

#endif