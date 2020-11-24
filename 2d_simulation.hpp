/*

	TODO:
	+	implement a Particle Interpolation, so we can put in a system some particles
	+	rework FDTD_2D algorithm & debug it 
	+	Esirkepov density decomposition
	+	Temperature distribution (have a descr.)
	+	reflecting conditions for a current
	-	description of density decomposition

*/

//-------- self-written libs -----------------------------------------------------------
#include "solvers/solvers.hpp"
#include "constants.h"

#ifndef VECTOR_CLASSES_H
#define VECTOR_CLASSES_H
	#include "vectors/vector_classes.hpp"
#endif

#ifndef VECTOR3_FIELD_H
#define VECTOR3_FIELD_H
	#include "vectors/vector3_field.hpp"
#endif

#ifndef PARTICLES_H
#define PARTICLES_H
	#include "particles/particles.hpp"
#endif

//-------- main libraries --------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cassert>
#include <chrono>
#include <ctime>
#include <cmath>
#include <omp.h>