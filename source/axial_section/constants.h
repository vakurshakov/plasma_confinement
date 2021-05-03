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
	const string initializer = "./manager_initializer.txt";
	const int 	THREAD_NUM = 8;

//######## MODIFIERS #################################################################
	#define there_are_particles 		true
	#define particles_are_diagnosed 	true
	#define fields_are_diagnosed 		true
	#define there_are_ion_current 		true
	#define there_are_magnetic_mirrors 	true

//######## PARTICLES CONSTANTS #######################################################
	const double mec2 	= 511;
	const double e 		= 1;
	const double me 	= 1;
	const double mpr 	= 1836;

//######## CONFIGURATION IN GENERAL ##################################################
	const int 	TIME	= 100;
	const int 	t_inj	= 25000;	// время нарастания сигнала
	const int 	diagnose_time_step = 2; 

	const int 	SIZE_X 	= 200;
	const int 	SIZE_Y 	= 400;
	const double dx 	= 0.04;
	const double dy		= 0.04;
	const double dt 	= 0.5*dx;
	
	const string boundaries = "px_ry";
	
	const double Np = 1;
	const double Tx = 30e-3;
	const double Ty = 30e-3;
	const double Tz = 30e-3;

	const string XY_distrib = "constant_flux_random";
//####################################################################################

	
//######## MAGNETIC MIRROR PARAMETERS ################################################
	const double Bz0  = 0.197;		// Bz0  = 0.2 [T]  { 0.197 }
	const double Bzl  = 0.1*Bz0; 
	const int 	nx_m1 = 0; 
	const int 	nx_m2 = SIZE_X; 
	const double x_m1 = nx_m1*dx;
	const double x_m2 = nx_m2*dx;
//####################################################################################


//######## PARTICLES DESCRIPTION #####################################################
	// тестовые параметры:			// ожидаемые параметры:
	const double n0 	= 1;		// n0   = 10e13	 [cm^(-3)]
	const double ni 	= 0.13;		// ni   = 1.291e11 [cm^(-3)]
	const double v_inj 	= 0.0565;	// Ek 	= 15 [keV] { 0.00565 } 
	const double r_larm	= 1.36; 	//1.36		// ож.: r_larm = 52,6 ( или 8,86 [cm] )
	const double r_prop	= 1.13;		// r_plasma/r_larm = 1.13
	const double dr		= 0.36;
	const double dr1	= 0.20;

	// Ширина плазменного столба в пробке 
	const double plasma_width = 2;
	
	// TODO: частицы без диагностик вызывают ошибку файловой системы!
	const multimap<string, vector<vector<string>>> species = {
			{ "Electrons", 
				{	
					{
						boundaries, to_string(n0), to_string(Np), XY_distrib,
						//cX 		  cY
						to_string(0), to_string(0),
						//Xm 			   Ym
						to_string(0.5*dx), to_string(0.5*dy),
						"vector3", "0 0 0"
			  		},
				
					{	"density"	},
				} 
			},
		};
//####################################################################################


//######## FIELDS DESCRIPTION ########################################################
	const vector<string> field_configuration = { boundaries, to_string(SIZE_X), to_string(SIZE_Y) };

	const multimap<string, vector<string>> field_diagnostics = {
		{ "whole_field", { "j", "x" } },
		{ "whole_field", { "j", "y" } },
		{ "whole_field", { "j", "z" } },
		// { "whole_field", { "E", "x" } },
		// { "whole_field", { "E", "y" } },
		// { "whole_field", { "E", "z" } },
		// { "whole_field", { "B", "x" } },
		// { "whole_field", { "B", "y" } },
		// { "whole_field", { "B", "z" } },

		//{ "field_along_X", { "j", "x", to_string(SIZE_Y/2) } },
		//{ "field_along_X", { "j", "y", to_string(SIZE_Y/2) } },
		//{ "field_along_X", { "E", "x", to_string(SIZE_Y/2) } },
		//{ "field_along_X", { "E", "y", to_string(SIZE_Y/2) } },
		//{ "field_along_X", { "E", "z", to_string(SIZE_Y/2) } },
		//{ "field_along_X", { "B", "x", to_string(SIZE_Y/2) } },
		//{ "field_along_X", { "B", "y", to_string(SIZE_Y/2) } },
		//{ "field_along_X", { "B", "z", to_string(SIZE_Y/2) } },

		//{ "field_along_Y", { "j", "x", to_string(SIZE_X/2) } },
		//{ "field_along_Y", { "j", "y", to_string(SIZE_X/2) } },
		//{ "field_along_Y", { "j", "z", to_string(SIZE_X/2) } },
		//{ "field_along_Y", { "E", "x", to_string(SIZE_X/2) } },
		//{ "field_along_Y", { "E", "y", to_string(SIZE_X/2) } },
		//{ "field_along_Y", { "E", "z", to_string(SIZE_X/2) } },
		//{ "field_along_Y", { "B", "x", to_string(SIZE_X/2) } },
		//{ "field_along_Y", { "B", "y", to_string(SIZE_X/2) } },
		//{ "field_along_Y", { "B", "z", to_string(SIZE_X/2) } },

		//{ "field_at_point", { "B", "x", to_string(SIZE_X/2), to_string(SIZE_Y/2) } }
		};
//####################################################################################


//######## SOLVERS ###################################################################
	const string field_solver = "FDTD_2D"; 
	
	// TODO: это вообще нужно будет прилепить к "species", но это просто, потому откладывается.
	const vector<string> particles_solvers = { "Boris_pusher",
											"Esirkepov_density_decomposition"};
//####################################################################################


//######## NAMING A DIRECTORY ########################################################
	const string dir_name = "../diagnostics/FRC/tests/" + boundaries + "/" 
	+ to_string(SIZE_X) + "Xcell_"
	+ to_string(dx).substr(0,4) + "dx/"
	+ to_string(int(Np)) + "ppc_" 
	+ XY_distrib
	//+ "injection_system_and_field_reversing/"
	+ "/Jz" + to_string(0.80*sqrt(Bz0 * 2*M_PI)/(dr1*dr1*dr1*dr1/4. + dr1*dr1*dr1/3. + dr1 - (dr1 - dr)*(dr1 - dr)*(dr1 - dr)*(dr1 - dr)/4.)).substr(0,5)
	//+ "_dr" + to_string(dr).substr(0,4) + "_dr1" + to_string(dr1).substr(0,4)
	+ "_R_LARM" + to_string(r_larm).substr(0,4)
	+ "/TIME" + to_string(TIME) + "_TINJ" + to_string(t_inj)
	+ "/" + to_string(SIZE_Y) + "Ycell";
//####################################################################################

//#################################################################################################

#endif
										   