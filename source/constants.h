#ifndef CONSTANTS_H
#define CONSTANTS_H

//#################################################################################################

#include <string>
#include <vector>
#include <cmath>
#include <map>

using std::vector, std::map, std::multimap, std::string, std::to_string;

//######## MODIFIERS #################################################################
	#define there_are_particles 			true
		#define there_are_ions				false
		#define there_are_electrons			true
		#define particles_are_diagnosed 	true

	#define there_are_fields				true
		#define there_are_Bz0				true	
		#define there_are_current_add 		false
		#define there_are_density_add 		false
		#define fields_are_diagnosed 		true


//######## PARTICLES CONSTANTS #######################################################
	inline const double mec2 = 511;
	inline const double e 	= 1;
	inline const double me 	= 1;
	inline const double mpr = 1836;
	
//######## CONFIGURATION IN GENERAL ##################################################
	inline const int THREAD_NUM = 16;

	inline const int TIME	= 1000;
	inline const int TINJ	= 5;		
	inline const int diagnose_time_step = 5; 

	inline const int SIZE_X = 100;
	inline const int SIZE_Y = 100;
	inline const double dx 	= 0.1;
	inline const double dy	= 0.1;
	inline const double dt 	= 0.5*dx;
	
	inline const string boundaries = "rx_ry";

//######## MAGNETIC MIRROR PARAMETERS ################################################
	inline const double Bz0  = 0.0197;		// Bz0  = 0.2 [T]  { 0.197 }

//######## PARTICLES DESCRIPTION #####################################################
	// тестовые параметры:			// ожидаемые параметры:
	inline const double n0 		= 1;		// n0   = 10e13	 [cm^(-3)]
	inline const int 	Npe		= 2;
	inline const double v_inj 	= 0.0589;	// Ek 	= 15 [keV] { 0.00565 } 
	inline const double r_larm	= 2;	 	// ож.: r_larm = 52,6 ( или 8,86 [cm] )
	inline const double r_prop	= 1.13;		// r_plasma/r_larm = 1.13
	inline const double dr		= 0.2;

	//зависимые параметры для обращения
	inline const int	 Npi	= 1;
	inline const double ni		= Bz0/( 2*dr*v_inj );	// ni   = 1.291e11 [cm^(-3)]
	inline const double mi		= r_larm*Bz0/v_inj;		//масса модельных частиц, чтобы выставить их на r_larm

	// TODO: частицы без диагностик вызывают ошибку файловой системы!
	#if there_are_particles
	inline const multimap<string, vector<vector<string>>> species = {
			#if there_are_electrons
			{ "electrons", 
				{	
					{	"Boris_pusher:+Push_particle",
						"Boris_pusher:+Interpolation;",
						"Esirkepov_density_decomposition",
						to_string(-e), to_string(me), to_string(n0), to_string(Npe),
						"circle", "random",
						to_string(30e-3), to_string(30e-3), to_string(0),
						"0"	},
				
					{	"density", "chosen_particles", "diagram_vx_on_y"	},
				} 
			},
			#endif
			
			#if there_are_ions
			{ "ions", 
				{	
					{	"Boris_pusher:+Push_particle;",
						"Concrete_point_interpolation: Homogenius_field E0=(0,0,0), B0=(0,0,"+to_string(Bz0)+");",
						"Esirkepov_density_decomposition",
						to_string(+e), to_string(mi), to_string(ni), to_string(Npi),
						"ring", "delayed",
						to_string(0), to_string(0), to_string(0),
						to_string(mi*v_inj/sqrt(1-v_inj*v_inj))	},
				
					{	"density"	},
				} 
			},
			{ "buffer_electrons", 
				{	
					{	"Boris_pusher:+Push_particle;",
						"Boris_pusher:+Interpolation;",
						"Esirkepov_density_decomposition",
						to_string(-e), to_string(me), to_string(ni), to_string(Npi),
						"ring", "delayed",
						to_string(0), to_string(0), to_string(0),
						to_string(0)	},
				
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
		{ "whole_field", { "j", "x" } },
		{ "whole_field", { "j", "y" } },
		{ "whole_field", { "E", "x" } },
		{ "whole_field", { "E", "y" } },
		{ "whole_field", { "B", "z" } },

		// { "field_along_x_axis", { "j", "x", to_string(SIZE_Y/2) } },
		// { "field_along_x_axis", { "j", "y", to_string(SIZE_Y/2) } },
		// { "field_along_x_axis", { "E", "x", to_string(SIZE_Y/2) } },
		// { "field_along_x_axis", { "E", "y", to_string(SIZE_Y/2) } },
		// { "field_along_x_axis", { "B", "z", to_string(SIZE_Y/2) } },

		// { "field_along_y_axis", { "j", "x", to_string(SIZE_X/2) } },
		// { "field_along_y_axis", { "j", "y", to_string(SIZE_X/2) } },
		// { "field_along_y_axis", { "E", "x", to_string(SIZE_X/2) } },
		// { "field_along_y_axis", { "E", "y", to_string(SIZE_X/2) } },
		// { "field_along_y_axis", { "B", "z", to_string(SIZE_X/2) } },

		// { "field_at_point", { "B", "z", to_string(SIZE_X/2), to_string(SIZE_Y/2) } }
		#endif
		};

//######## SOLVERS ###################################################################
	inline const string field_solver = "FDTD_2D"; 

//######## NAMING A DIRECTORY ########################################################
	inline const string dir_name = "./diagnostics/ionization/tests/testing_chosen_particles";

//#################################################################################################

#endif
										     