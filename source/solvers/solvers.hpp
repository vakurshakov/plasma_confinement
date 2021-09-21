#ifndef SOLVERS_H
#define SOLVERS_H

//#################################################################################################

#include "../particle/particle_parameters.hpp"
#include "../particle/point.hpp"
#include "../vectors/vector_classes.hpp"
#include "../vectors/vector3_field.hpp"

void Esirkepov_density_decomposition(const Particle_parameters&,
									 const Point&,
									 const vector2& r0,
									 vector3_field* J);

void FDTD_2D(vector3_field& E, vector3_field& B, vector3_field& J);

//#################################################################################################

#endif