#ifndef CONSTANTS_H
#define CONSTANTS_H

//#################################################################################################

#include <string>
#include <vector>

using namespace std;

//-------- particles constants ---------------------------------------------------------
	const double mec2 	= 511;
	const double e 		= 1;
	const double me 	= 1;
	const double mpr 	= 1836;

//-------- initializer -----------------------------------------------------------------
	const string initializer = "./manager_initializer.txt";

//-------- diagnostics -----------------------------------------------------------------
	const string head_folder = "../diagnostics/circular_current_test";
	const vector<string> names_of_diagnostics = { "energy",
												  "field_configuration",
												  "field_at_point",
												  "phase_diagram" };

//-------- solvers ---------------------------------------------------------------------
	const string field_solver = "FDTD_2D"; 
	const vector<string> particles_solvers = { "Boris_pusher",
											"Esirkepov_density_decomposition"};

//-------- configuration ---------------------------------------------------------------
	const int 	SIZE_X 	= 100,	SIZE_Y 	= 100;
	const int 	TIME	= 51;
	const double dx 	= 0.04, dy 		= 0.04;
	const double dt 	= 0.02;
	const string boundaries = "reflective";
	
	const double Np = 8;
	const double Tx = 1;
	const double Ty = 1;
	const double Tz = 0;

	const string XY_distrib = "circle";
	const double n0 	= 1;		// plasma density according to ions, n0
	const double ni 	= 4428;		// ion density, ni
	const double v_inj 	= 0.00565;
	const double Bz0 	= 1;

	const int spline_width = 4;

	const vector<string> field_configuration = { to_string(SIZE_X), to_string(SIZE_Y), boundaries };
	const vector<string> particles_configuration = { boundaries, to_string(n0), to_string(ni),
			to_string(Np), XY_distrib };	

//#################################################################################################

#endif
										   