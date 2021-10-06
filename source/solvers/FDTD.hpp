#ifndef FDTD_HPP
#define FDTD_HPP

//#################################################################################################

#include "../particles/particle/particle_parameters.hpp"
#include "../particles/particle/point.hpp"
#include "../vectors/vector_classes.hpp"
#include "../vectors/vector3_field.hpp"

void FDTD_2D(vector3_field& E, vector3_field& B, vector3_field& J);

//#################################################################################################

#endif // FDTD_HPP