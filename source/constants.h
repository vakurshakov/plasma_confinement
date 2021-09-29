#ifndef CONSTANTS_H
#define CONSTANTS_H

//#################################################################################################

#include <string>
#include <vector>
#include <cmath>
#include <map>

using std::vector, std::map, std::multimap, std::string, std::to_string;

	//const double M_PI = 3.14159265358979323846;

//######## MODIFIERS #################################################################
	#define there_are_particles 			true
		#define there_are_ions				true
		#define there_are_electrons			true
		#define particles_are_diagnosed 	true

	#define there_are_fields				true
		#define there_are_Bz0				true
		#define there_are_current_add 		true
		#define there_are_density_add 		true
		#define fields_are_diagnosed 		true


//######## PARTICLES CONSTANTS #######################################################
	const double mec2 	= 511;
	const double e 		= 1;
	const double me 	= 1;
	const double mpr 	= 1836;
	
//######## CONFIGURATION IN GENERAL ##################################################
	const int 	THREAD_NUM = 16;


	const int 	TIME	= 5;
	const int 	TINJ	= 25000;		// время нарастания тока ионов (пока никак не участвует)
	const int 	diagnose_time_step = 10; 

	const int 	SIZE_X 	= 160;
	const int 	SIZE_Y 	= 160;
	const double dx 	= 0.04;
	const double dy		= 0.04;
	const double dt 	= 0.5*dx;
	
	const string boundaries = "rx_ry";

//######## MAGNETIC MIRROR PARAMETERS ################################################
	const double Bz0  = 0.0197;		// Bz0  = 0.2 [T]  { 0.197 }

//######## PARTICLES DESCRIPTION #####################################################
	// тестовые параметры:			// ожидаемые параметры:
	const double n0 	= 1;		// n0   = 10e13	 [cm^(-3)]
	const int Npe 		= 5;
	const double v_inj 	= 0.00591;	// Ek 	= 15 [keV] { 0.00565 } 
	const double r_larm	= 1.36;	 	//3 (курсовая) // ож.: r_larm = 52,6 ( или 8,86 [cm] )
	const double r_prop	= 1.13;		// r_plasma/r_larm = 1.13
	//const double dr		= 0.48;
	const double dr		= 0.24;


	//зависимые параметры для обращения
	const double ni 	= Bz0/( 2*dr*v_inj );	// ni   = 1.291e11 [cm^(-3)]
	const int Npi 		= 5;
	const double mi 	= r_larm*Bz0/v_inj;		//масса модельных частиц, чтобы выставить их на r_larm


	// TODO: частицы без диагностик вызывают ошибку файловой системы!
	#if there_are_particles
	const multimap<string, vector<vector<string>>> species = {
			#if there_are_electrons
			{ "electrons", 
				{	
					{	"Boris_pusher:+Interpolation,+Push_particle", "Esirkepov_density_decomposition",
						to_string(-e), to_string(me), to_string(n0), to_string(Npe),
						"circle", "random",
						to_string(30e-3), to_string(30e-3), to_string(0),
						"0"	},
				
					{	"density"	},
				} 
			},
			#endif
			
			#if there_are_ions
			{ "ions", 
				{	
					{	"Boris_pusher:+Push_particle", "Esirkepov_density_decomposition",
						to_string(+e), to_string(mi), to_string(ni), to_string(Npi),
						"", "",
						to_string(30e-3), to_string(30e-3), to_string(0),
						to_string(mi*v_inj/sqrt(1-v_inj*v_inj))	},
				
					{	"density"	},
				} 
			},
			{ "buffer_electrons", 
				{	
					{	"Boris_pusher:+Interpolation,+Push_particle", "Esirkepov_density_decomposition",
						to_string(-e), to_string(me), to_string(ni), to_string(Npi),
						"", "",
						to_string(30e-3), to_string(30e-3), to_string(0),
						to_string(0)	},
				
					{	"density"	},
				} 
			},
			#endif
		};
	#endif

//######## FIELDS DESCRIPTION ########################################################
	const vector<string> field_configuration = { boundaries, to_string(SIZE_X), to_string(SIZE_Y) };

	const multimap<string, vector<string>> fields_diagnostics = {
		{ "whole_field", { "j", "x" } },
		{ "whole_field", { "j", "y" } },
		{ "whole_field", { "E", "x" } },
		{ "whole_field", { "E", "y" } },
		{ "whole_field", { "B", "z" } },

		{ "field_along_x_axis", { "j", "x", to_string(SIZE_Y/2) } },
		{ "field_along_x_axis", { "j", "y", to_string(SIZE_Y/2) } },
		{ "field_along_x_axis", { "E", "x", to_string(SIZE_Y/2) } },
		{ "field_along_x_axis", { "E", "y", to_string(SIZE_Y/2) } },
		{ "field_along_x_axis", { "B", "z", to_string(SIZE_Y/2) } },

		{ "field_along_y_axis", { "j", "x", to_string(SIZE_X/2) } },
		{ "field_along_y_axis", { "j", "y", to_string(SIZE_X/2) } },
		{ "field_along_y_axis", { "E", "x", to_string(SIZE_X/2) } },
		{ "field_along_y_axis", { "E", "y", to_string(SIZE_X/2) } },
		{ "field_along_y_axis", { "B", "z", to_string(SIZE_X/2) } },

		{ "field_at_point", { "B", "z", to_string(SIZE_X/2), to_string(SIZE_Y/2) } }
		};

//######## SOLVERS ###################################################################
	const string field_solver = "FDTD_2D"; 

//######## NAMING A DIRECTORY ########################################################
	const string dir_name = "./diagnostics/ionizaton/test";

//#################################################################################################

#endif
										   