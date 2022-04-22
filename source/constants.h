#ifndef CONSTANTS_H
#define CONSTANTS_H

//#################################################################################################

#include <string>
#include <vector>
#include <cmath>
#include <map>

using std::vector, std::map, std::multimap, std::string, std::to_string;

//######## MODIFIERS #################################################################
	#define there_are_particles 					true
		#define there_are_electrons					false
		#define there_are_plasma_ions				false
		#define there_are_ions						true
		#define density_beam_profile_is_set 		false
		#define particles_are_diagnosed 			true

	#define there_are_fields				true
		#define there_are_Bz0				true	
		#define fields_are_diagnosed 		true
		//#define there_are_current_add 		false
		//#define there_are_density_add 		false


//######## PARTICLES CONSTANTS #######################################################
	inline const double e 	= 1.;
	inline const double me 	= 1.;
	inline const double Mp  = 1836.;
	
//######## CONFIGURATION IN GENERAL ##################################################
	inline const int THREAD_NUM = 5;

	inline const int TINJ	= 1;		
	inline const int diagnose_time_step = 1; 

	inline const int TIME	= 10;
	inline const int SIZE_X = 1000;
	inline const int SIZE_Y = 1000;
	inline const double dx 	= 0.1;
	inline const double dy	= 0.1;
	inline const double dt 	= 0.5*dx;
	
	inline const string boundaries = "rx_ry";

//######## MAGNETIC MIRROR PARAMETERS ################################################
	inline const double Bz0  = 0.0197;		// Bz0  = 0.2 [T]  { 0.197 }

//######## PARTICLES DESCRIPTION #####################################################
	// тестовые параметры:			// ожидаемые параметры:
	inline const double n0 		= 1.;		// n0   = 10e13	 [cm^(-3)]
	inline const int 	Npe		= 10;
	inline const double v_inj 	= 0.00589;	// Ek 	= 15 [keV] { 0.00565 } 
	inline const double r_larm	= 52.6 / 2.;// ож.: r_larm = 52,6 ( или 8,86 [cm] )
	inline const double r_prop	= 1.13;		// r_plasma/r_larm = 1.13
	inline const double dr		= 3.;

	//зависимые параметры для обращения
	inline const int   Npi		= 100;
	inline const double ni		= 1.;	// ni   = 1.291e11 [cm^(-3)]
	inline const double mi		= Mp;

	// TODO: частицы без диагностик вызывают ошибку файловой системы!
	#if there_are_particles
	inline const multimap<string, vector<vector<string>>> species = {
			#if there_are_electrons
			{ "plasma_electrons", 
				{
					{	"Boris_pusher:+Push_particle",
						"Boris_pusher:+Interpolation;",
						"Esirkepov_density_decomposition",
						"global, " + to_string(n0),
						"global, " + to_string(-e), to_string(me), to_string(Npe),
						"set", "circle", "random",
						"30e-3", "30e-3", "0",
						"0"	},
				
					{	"density"	},
				} 
			},
			#endif

			#if there_are_plasma_ions
			{ "plasma_ions",
				{
					{	"Boris_pusher:+Push_particle",
						"Boris_pusher:+Interpolation;",
						"Esirkepov_density_decomposition",
						"global, " + to_string(n0),
						"global, " + to_string(+e), to_string(Mp), to_string(Npe),
						"copy_coordinates_from_plasma_electrons", "circle", "random",
						"30e-3", "30e-3", "0",
						"0"	},
				
					{	"density"	},
				}
			},
			#endif
			
			#if there_are_ions
			{ "ions", 
				{	
					{	"Boris_pusher:+Push_particle;",
						"Concrete_point_interpolation: Homogenius_field E0=(0,0,0), B0=(0,0,"+to_string(Bz0)+");",
						"Esirkepov_density_decomposition",
						
						#if density_beam_profile_is_set
							"local",
						#else 
							"global, " + to_string(ni),
						#endif

						"global, " + to_string(+e), to_string(mi), to_string(Npi),
						"ionize_particles", "ring", "random",
						"0", "0", "0",
						to_string(mi * v_inj / sqrt(1. - v_inj * v_inj))	},
				
					{	"density"	},
				} 
			},
			{ "buffer_electrons", 
				{	
					{	"Boris_pusher:+Push_particle;",
						"Boris_pusher:+Interpolation;",
						"Esirkepov_density_decomposition",
						
						#if density_beam_profile_is_set
							"local",
						#else 
							"global, " + to_string(ni),
						#endif
						
						"global, " + to_string(-e), to_string(me), to_string(Npi),
						"ionize_particles", "ring", "random",
						"30e-3", "30e-3", "0",
						to_string(me * v_inj / sqrt(1. - v_inj * v_inj))	},
				
					{	"density"	},
				} 
			},
			#endif
		};
	#endif

//######## FIELDS DESCRIPTION ########################################################
	inline const vector<string> field_configuration = { boundaries, to_string(SIZE_X), to_string(SIZE_Y) };

	inline const multimap<string, vector<string>> fields_diagnostics = {
		#if fields_are_diagnosed
		{ "whole_field", { "E", "x" } },
		{ "whole_field", { "E", "y" } },
		{ "whole_field", { "B", "z" } },
	
		// { "field_along_x_axis", { "E", "x", to_string(SIZE_Y/2) } },
		// { "field_along_x_axis", { "E", "y", to_string(SIZE_Y/2) } },
		// { "field_along_x_axis", { "B", "z", to_string(SIZE_Y/2) } },

		// { "field_along_y_axis", { "E", "x", to_string(SIZE_X/2) } },
		// { "field_along_y_axis", { "E", "y", to_string(SIZE_X/2) } },
		// { "field_along_y_axis", { "B", "z", to_string(SIZE_X/2) } },

		// { "field_at_point", { "B", "z", to_string(SIZE_X/2), to_string(SIZE_Y/2) } }
		#endif
		};

//######## SOLVERS ###################################################################
	inline const string field_solver = "FDTD_2D"; 

//######## NAMING A DIRECTORY ########################################################
	inline const string dir_name = "./results/tests/beam_profile";

//#################################################################################################

#endif
										      
