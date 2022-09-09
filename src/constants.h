#ifndef SRC_CONSTANTS_H
#define SRC_CONSTANTS_H

#include <string>
#include <vector>
#include <cmath>
#include <map>

using std::vector, std::map, std::multimap, std::string, std::to_string;

//######## MODIFIERS #################################################################
	#define there_are_particles 			true
		#define there_are_electrons			true
		#define there_are_plasma_ions		true
		#define there_are_ions				true
		#define density_beam_profile_is_set false
		#define particles_are_diagnosed 	true

	#define there_are_fields				true
		#define there_are_Bz0				true
		#define fields_are_diagnosed 			true

	inline const double e 	= 1.;
	inline const double me 	= 1.;
	inline const double Mp  = 1836.;
	
//######## CONFIGURATION IN GENERAL ##################################################
	inline const string dir_name = "./results/particles_in_segment";
	inline const int THREAD_NUM = 1;

	inline const double dx 	= 0.05;
	inline const double dy	= 0.05;
	inline const double dt 	= 0.5 * dx;

	inline const int TIME	= 1000000;
	inline const int SIZE_X = 3000;
	inline const int SIZE_Y = 3000;
	inline const int TINJ	= 1000 / dt;
	inline const int diagnose_time_step = 200;
	
	inline const string boundaries = "rx_ry";

	inline const double Bz0  = 0.8;		// Bz0  = 0.2 [T]  { 0.197 }

//######## PARTICLES DESCRIPTION #####################################################
	// тестовые параметры:			// ожидаемые параметры:
	inline const double n0 		= 1.;		// n0   = 10e13	 [cm^(-3)]
	inline const int    Npe		= 10;
	inline const double r_larm	= 5.0;		// ож.: r_larm = 52,6 ( или 8,86 [cm] )
	inline const double r_prop	= 1.;		// r_plasma/r_larm = 1.13
	inline const double dr		= 5.0;		// r_plasma = (r_larm + dr) * r_prop
	inline const int   Npi		= 25;
	inline const double ni		= 10.;		// ni   = 1.291e11 [cm^(-3)]

	//зависимые параметры для обращения
	inline const double v_inj 	= r_larm * (2 * M_PI) / 1000.;	// Ek 	= 15 [keV] { 0.00565 } 
	inline const double mi		= Bz0 / (2 * M_PI) * 1000.;

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
						"50e-3", "50e-3", "50e-3",
						"0"	},
				
					{	"density", "first_Vphi_moment", "chosen_particles"	},
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
						"50e-3", "50e-3", "50e-3",
						"0"	},
				
					{	"density", "first_Vphi_moment", "chosen_particles"	},
				}
			},
			#endif
			
			#if there_are_ions
			{ "beam_ions", 
				{	
					{	"Boris_pusher:+Push_particle;",
						"Concrete_point_interpolation: Homogenius_field E0=(0,0,0), B0=(0,0,"+to_string(Bz0)+");",  // ионы пучка на ларморовской окружности
						// "Boris_pusher:+Interpolation;", // ионы пучка свободны
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
				
					{	"density", "first_Vphi_moment", "chosen_particles"	},
				} 
			},
			{ "beam_electrons", 
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
						"10e-3", "10e-3", "10e-3",
						to_string(me * v_inj / sqrt(1. - v_inj * v_inj))	},
				
					{	"density", "first_Vphi_moment", "chosen_particles"	},
				} 
			},
			#endif
		};
	#endif

namespace config {

inline const multimap<string, vector<string>> fields_diagnostics = {
#if fields_are_diagnosed
  { "whole_field", { "E", "x", "1000", "2000", "1000", "2000" } },
  { "whole_field", { "E", "y", "1000", "2000", "1000", "2000" } },
  { "whole_field", { "j", "x", "1000", "2000", "1000", "2000" } },
  { "whole_field", { "j", "y", "1000", "2000", "1000", "2000" } },
  { "whole_field", { "B", "z", "1000", "2000", "1000", "2000" } },
#endif
};

inline const double absorbing_layer_width = 3;
inline const double absorption_factor = 0.1;

}  // namespace config

#endif  // SRC_CONSTANTS_H
