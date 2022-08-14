#ifndef FDTD_HPP
#define FDTD_HPP

#include "src/pch.h"
#include "src/fields/em_fields.hpp"

void FDTD_2D(electric_field& E, magnetic_field& B, electric_field& J);

#endif  // FDTD_HPP
