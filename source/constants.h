#ifndef CONSTANTS_H
#define CONSTANTS_H

//#################################################################################################

#include <string>
#include <vector>
#include <map>

using namespace std;

	// Название пуска
	const string dir_name = "../diagnostics/vacuum_test";

//-------- particles constants --------------------------------------------------------------------
	const double mec2 	= 511;
	const double e 		= 1;
	const double me 	= 1;
	const double mpr 	= 1836;

	const int 	TIME	= 100;
//-------- field configuration --------------------------------------------------------------------
	const int 	SIZE_X 	= 64,	SIZE_Y 	= 64;
	const double dx 	= 0.04, dy 		= 0.04;
	const double dt 	= 0.02;
	const string boundaries = "reflective";
	
	const vector<string> field_configuration = { boundaries, to_string(SIZE_X), to_string(SIZE_Y) };
	
	const double Np = 4;
	const double Tx = 30e-3;
	const double Ty = 30e-3;
	const double Tz = 0;

	const string XY_distrib = "circle_random";
									// ожидаемые параметры:
	const double n0 	= 1;		// n0   = 10e13	 [cm^(-3)]
	const double ni 	= 0.013;	// ni   = 1.291e11 [cm^(-3)]
	const double v_inj 	= 0.00565;	// Ek 	= 15 [keV] 
	const double Bz0 	= 0.197;	// Bz0  = 0.2 [T]
	const double r_larm = 0.6;		// ож.: r_larm = 52,6 ( или 8,86 [cm] )
	const double r_prop = 1.13;		// r_plasma/r_larm = 1.13
	const double t_inj 	= 50.;		// время нарастания сигнала
	const double ds 	= 0.04;

	const int spline_width = 4;

	// TODO: частицы без диагностик вызывают ошибку файловой системы!
	const multimap<string, vector<vector<string>>> species = {
		{ "Electrons", { { boundaries, to_string(n0), to_string(Np), XY_distrib, "vector2", "0 0"},
						 { "energy" } } },
		};	

//-------- initializer ----------------------------------------------------------------------------
	const string initializer = "./manager_initializer.txt";

//-------- diagnostics ----------------------------------------------------------------------------	
	const multimap<string, vector<string>> field_diagnostics = {
		{ "energy", { "" } },
		//{ "whole_field", { "j", "x" } },
		//{ "whole_field", { "j", "y" } },
		{ "whole_field", { "E", "x" } },
		{ "whole_field", { "E", "y" } },
		{ "whole_field", { "E", "z" } },
		{ "whole_field", { "B", "x" } },
		{ "whole_field", { "B", "y" } },
		{ "whole_field", { "B", "z" } },
		//{ "field_along_Y", { "j", "x", to_string(SIZE_X/2) } },
		// { "field_at_point", { "B", "3", to_string(int(SIZE_X/2)), to_string(int(SIZE_Y/2)) } }
		};	

//-------- solvers --------------------------------------------------------------------------------
	const string field_solver = "FDTD_2D"; 
	
	// TODO: это вообще нужно будет прилепить к "species", но это просто, потому откладывается.
	const vector<string> particles_solvers = { "Boris_pusher",
											"Esirkepov_density_decomposition"};

//#################################################################################################

#endif
										   