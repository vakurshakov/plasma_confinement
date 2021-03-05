#ifndef SOLVERS_H
#define SOLVERS_H

//#################################################################################################

#include "../vectors/vector3_field.hpp"
#include "../particles/species_description.hpp"


void Boris_pusher(const Species_description& sort, Particle& particle,
				  const vector3_field& E, const vector3_field& B);

void Esirkepov_density_decomposition(const Species_description& sort,
									 const Particle& particle,
									 const vector2& r0,
									 vector3_field& J);

void FDTD_2D(vector3_field& E, vector3_field& B, vector3_field& j);

//#################################################################################################

#endif