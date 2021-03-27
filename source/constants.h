#ifndef CONSTANTS_H
#define CONSTANTS_H

//#################################################################################################

#include <iomanip>
#include <chrono>
#include <string>
#include <vector>
#include <cmath>
#include <map>

using namespace std;

	const double M_PI = 3.14159265358979323846;

//-------- particles constants --------------------------------------------------------------------
	const double mec2 	= 511;
	const double e 		= 1;
	const double me 	= 1;
	const double mpr 	= 1836;

	const int 	TIME	= 10000;
	const int 	diagnose_time_step = 50; 
	const int 	THREAD_NUM = 8;
//-------- field configuration --------------------------------------------------------------------
	const int 	SIZE_X 	= 64;  // 5.12 c/wp 
	const int 	SIZE_Y 	= 64;  // 5.12 c/wp
	const double dx 	= 0.04; 
	const double dy		= 0.04;
	const double dt 	= 0.02;
	const string boundaries = "reflective";
	
	const vector<string> field_configuration = { boundaries, to_string(SIZE_X), to_string(SIZE_Y) };

	
	const double Np = 1;
	const double Tx = 30e-3;
	const double Ty = 30e-3;
	const double Tz = 0;

	const string XY_distrib = "circle_random";
									// ожидаемые параметры:
	const double n0 	= 1;		// n0   = 10e13	 [cm^(-3)]
	const double ni 	= 0.13;		// ni   = 1.291e11 [cm^(-3)]
	const double v_inj 	= 0.0565;	// Ek 	= 15 [keV] { 0.00565 } 
	const double Bz0 	= 0.197;	// Bz0  = 0.2 [T]
	const double r_larm = 0.6;		// ож.: r_larm = 52,6 ( или 8,86 [cm] )
	const double r_prop = 1.13;		// r_plasma/r_larm = 1.13
	const int t_inj 	= 5000;		// время нарастания сигнала
	const double dr 	= 0.12;

	const int spline_width = 4;

	const string dir_name = "../diagnostics/FRC/" + boundaries + "/" + to_string(SIZE_X) + "Xcell/"
	+ to_string(int(Np)) + "ppc_" + XY_distrib
	+ "_j" + to_string(0.25*M_PI*Bz0/dr).substr(0,5) + "_dr" + to_string(dr).substr(0,4)
	+ "_Bz0" + to_string(Bz0).substr(0,4)
	+ "_TIME" + to_string(TIME) + "_TINJ" + to_string(t_inj)
	+ "/injection_system_and_field_reversing";


	// TODO: частицы без диагностик вызывают ошибку файловой системы!
	const multimap<string, vector<vector<string>>> species = {
		{ "Electrons", { { boundaries, to_string(n0), to_string(Np), XY_distrib, "vector2", "0 0"},
						 { "density" } } },
		};	

//-------- initializer ----------------------------------------------------------------------------
	const string initializer = "./manager_initializer.txt";

//-------- diagnostics ----------------------------------------------------------------------------	
	const multimap<string, vector<string>> field_diagnostics = {
		{ "whole_field", { "j", "x" } },
		{ "whole_field", { "j", "y" } },
		{ "whole_field", { "E", "x" } },
		{ "whole_field", { "E", "y" } },
		//{ "whole_field", { "E", "z" } },
		//{ "whole_field", { "B", "x" } },
		//{ "whole_field", { "B", "y" } },
		{ "whole_field", { "B", "z" } },

		{ "field_along_X", { "j", "x", to_string(SIZE_Y/2) } },
		{ "field_along_X", { "j", "y", to_string(SIZE_Y/2) } },
		{ "field_along_X", { "E", "x", to_string(SIZE_Y/2) } },
		{ "field_along_X", { "E", "y", to_string(SIZE_Y/2) } },
		//{ "field_along_X", { "E", "z", to_string(SIZE_Y/2) } },
		//{ "field_along_X", { "B", "x", to_string(SIZE_Y/2) } },
		//{ "field_along_X", { "B", "y", to_string(SIZE_Y/2) } },
		{ "field_along_X", { "B", "z", to_string(SIZE_Y/2) } },

		{ "field_along_Y", { "j", "x", to_string(SIZE_X/2) } },
		{ "field_along_Y", { "j", "y", to_string(SIZE_X/2) } },
		{ "field_along_Y", { "E", "x", to_string(SIZE_X/2) } },
		{ "field_along_Y", { "E", "y", to_string(SIZE_X/2) } },
		//{ "field_along_Y", { "E", "z", to_string(SIZE_X/2) } },
		//{ "field_along_Y", { "B", "x", to_string(SIZE_X/2) } },
		//{ "field_along_Y", { "B", "y", to_string(SIZE_X/2) } },
		{ "field_along_Y", { "B", "z", to_string(SIZE_X/2) } },

		{ "field_at_point", { "B", "z", to_string(SIZE_X/2), to_string(SIZE_Y/2) } }
		};

//-------- solvers --------------------------------------------------------------------------------
	const string field_solver = "FDTD_2D"; 
	
	// TODO: это вообще нужно будет прилепить к "species", но это просто, потому откладывается.
	const vector<string> particles_solvers = { "Boris_pusher",
											"Esirkepov_density_decomposition"};

//#################################################################################################

#endif
										   