#ifndef CONSTANTS_H
#define CONSTANTS_H

//#################################################################################################

#include <string>
#include <vector>
#include <map>

using namespace std;

	// Название пуска
	const string test_name = "../diagnostics/circular_current_test";

//-------- particles constants --------------------------------------------------------------------
	const double mec2 	= 511;
	const double e 		= 1;
	const double me 	= 1;
	const double mpr 	= 1836;

	const int 	TIME	= 3000;
//-------- field configuration --------------------------------------------------------------------
	const int 	SIZE_X 	= 32,	SIZE_Y 	= 32;
	const double dx 	= 0.04, dy 		= 0.04;
	const double dt 	= 0.02;
	const string boundaries = "reflective";
	
	const vector<string> field_configuration = { boundaries, to_string(SIZE_X), to_string(SIZE_Y) };
	
	const double Np = 8;
	const double Tx = 0.1;
	const double Ty = 0.1;
	const double Tz = 0;

	const string XY_distrib = "circle_random";
	const double n0 	= 1;		// plasma density according to ions, n0
	const double ni 	= 4428;		// ion density, ni
	const double v_inj 	= 0.00565;
	const double Bz0 	= 1;
	const double r_larm = 10;		// это просто число, нужно посчитать аккуратнее

	const int spline_width = 4;

	const map<string, vector<vector<string>>> species = {
		{ "Electrons", { { boundaries, to_string(n0), to_string(Np), XY_distrib, "vector2", "0 0"},
						 { "energy", "phase_diagram" } } },
		{ "Protons -d", { { to_string(ni) },
						  { "" } } }
		};	

//-------- initializer ----------------------------------------------------------------------------
	const string initializer = "./manager_initializer.txt";

//-------- diagnostics ----------------------------------------------------------------------------	
	const map<string, vector<string>> field_diagnostics = {
		{ "energy", { "" } },
		{ "whole_field", { "j", "1" } },
		//{ "field_at_point", { "B", "3", to_string(int(SIZE_X/2)), to_string(int(SIZE_Y/2)) } }
		};	

//-------- solvers --------------------------------------------------------------------------------
	const string field_solver = "FDTD_2D"; 
	
	// TODO: это вообще нужно будет прилепить к "species", но это просто, потому откладывается.
	const vector<string> particles_solvers = { "Boris_pusher",
											"Esirkepov_density_decomposition"};

//#################################################################################################

#endif
										   