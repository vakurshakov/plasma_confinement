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

void Boris_pusher(const sort_of_particles& SORT, particle& PARTICLE,
				  const vector3_field& E, const vector3_field& B);

void Esirkepov_density_decomposition(const sort_of_particles& SORT,
									 const particle& PARTICLE,
									 const vector2& r0,
									 vector3_field& J);

void FDTD_2D(vector3_field& E, vector3_field& B, vector3_field& j);
