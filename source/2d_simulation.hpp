//-------- self-written libs -----------------------------------------------------------
#ifndef CONSTANTS_H
#define CONSTANTS_H
	#include "./constants.h"
#endif

#ifndef VECTOR_CLASSES_H
#define VECTOR_CLASSES_H
	#include "./vectors/vector_classes.hpp"
#endif

#ifndef VECTOR3_FIELD_H
#define VECTOR3_FIELD_H
	#include "./vectors/vector3_field.hpp"
#endif

#ifndef PARTICLES_H
#define PARTICLES_H
	#include "./particles/class_particles.hpp"
#endif

#ifndef SOLVERS_H
#define SOLVERS_H
	#include "./solvers/solvers_manager.hpp"
#endif

#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H
	#include "./diagnostics/diagnostics.hpp" 
#endif

#ifndef MANAGER_H
#define MANAGER_H
	#include "./manager.hpp"
#endif

//-------- main libraries --------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <chrono>
#include <ctime>
#include <cmath>
#include <omp.h>