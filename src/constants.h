#ifndef SRC_CONSTANTS_H
#define SRC_CONSTANTS_H

//! @todo Refactor constants.h to contain only the global
//! constants other constants should be represented by some
//! name-accessible class (equivalent to map).

#include <cmath>

#include <string>
#include <vector>
#include <unordered_map>

#define there_are_particles             true
  #define GLOBAL_DENSITY                true
  #define particles_are_diagnosed       true
  #define there_are_plasma_ions         true
  #define there_are_plasma_electrons    true

#define there_are_fields                true
  #define fields_are_diagnosed          true
  #define there_are_Bz0                 true

#define _2D3V                           false

#define LOGGING                         true
#define TIME_PROFILING                  true
#define MAKE_BACKUPS                    true
#define START_FROM_BACKUP               false


#define BEAM_INJECTION_SETUP            true
#if BEAM_INJECTION_SETUP && !(GLOBAL_DENSITY && there_are_plasma_electrons)
  #error "Beam injection setup only works with global density!"
#endif


inline const double e   = 1.0;
inline const double me  = 1.0;
inline const double Mp  = 1836.0;

inline const std::string dir_name = "./results/test_dir_name";
inline const int NUM_THREADS = 16;

inline const double dx = 0.02;  // for Te = 0.2 keV

#if !BEAM_INJECTION_SETUP
inline const int SIZE_X = 5'200;
#else
inline const int SIZE_X = 10'200;
#endif

inline const double dy  = dx;
inline const int SIZE_Y = 2'000;

inline const double dt = 0.5 * dx;
inline const int TIME  = 500'000;

inline const int diagnose_time_step = 250;

namespace config {

// (transition layer width) = 800.0 * dx
inline const double layer_beginning = 0.5 * (SIZE_X * dx - 40.0);

inline const double n0 = 1.0;
inline const int   Npi = 16;

inline const int BUFFER_SIZE = 2;

inline const double mi_me  = 16.0;

inline const double T_ions = 10.0;  // KeV
inline const double V_ions = sqrt(T_ions / mi_me / 511.0);

inline const double Omega_max = sqrt(2 * T_ions / 511.0);

inline const std::string postfix = "0.01dx_16.0mi_me_10.0Ti.bin";

inline const std::string boundaries = "cx_py";

inline const int damping_layer_width = 100;
inline const double damping_factor = 0.8;

#if !BEAM_INJECTION_SETUP
inline const double density_limit = 0.0001;

inline const double T_electrons = 0.2;  // KeV
inline const double V_electrons = sqrt(T_electrons / me / 511.0);

#else
inline const int INJECTION_START = 0;
inline const int INJECTION_TIME = 100'000;

inline const int WIDTH_OF_INJECTION_AREA = 1250;
inline const int PER_STEP_PARTICLES = SIZE_Y * WIDTH_OF_INJECTION_AREA * Npi / INJECTION_TIME;

inline const double T_electrons = 0.2;  // KeV
inline const double V_electrons = sqrt(T_electrons / me / 511.0);

#endif

// Domain_geometry
inline const double domain_left   = damping_layer_width * dx;
inline const double domain_right  = (SIZE_X - damping_layer_width) * dx;
inline const double domain_bottom = 0;
inline const double domain_top    = SIZE_Y * dy;


template<class K, class V>
using umap = std::unordered_multimap<K, V>;
using std::to_string;

inline const umap<std::string,
  umap<std::string, std::vector<std::string>>> species_description = {
#if there_are_particles

#if there_are_plasma_ions
  { "plasma_ions", {
    { "parameters", {
        to_string(n0),      // Particle density [in units of n₀]
        to_string(+e),      // Particle charge [in units of e]
        to_string(mi_me),   // Particle mass [in units mₑ]
        to_string(Npi),     // Number of particles representing the density n₀
        to_string(T_ions),  // Temperature in x, y and z direction [in KeV]
        to_string(T_ions),  //
        "0",                //
        "0"                 // Absolute value of the initial impulse [in units of mₑc]
    }},
    { "integration_steps", {
        "Boris_pusher",
        "Simple_interpolation",
        "Esirkepov_density_decomposition",
    }},
    // Diagnostics with their config parameters
    { "energy", { "empty description" }},
    { "density", {                                                  //
      "0", "0",                                                     // minimal captured coordinate [in units of c/ωₚ]
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),               // maximum captured coordinate [in units of c/ωₚ]
      to_string(dx), to_string(dy),                                 // step between nearest coordinates [in units of c/ωₚ]
    }},
    { "Vy_moment", {                                                //
      "0", "0",                                                     // minimal captured coordinate [in units of c/ωₚ]
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),               // maximum captured coordinate [in units of c/ωₚ]
      to_string(dx), to_string(dy),                                 // step between nearest coordinates [in units of c/ωₚ]
    }},
    { "mVxVx_moment", {                                             //
      "0", "0",                                                     // minimal captured coordinate [in units of c/ωₚ]
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),               // maximum captured coordinate [in units of c/ωₚ]
      to_string(dx), to_string(dy),                                 // step between nearest coordinates [in units of c/ωₚ]
    }},
    { "x0_distribution_function", {                                 //
      to_string(SIZE_X / 2),                                        // x0 [in cells]
      to_string(-10 * V_ions), to_string(-10 * V_ions),             // minimal captured velocity [in units of c]
      to_string(+10 * V_ions), to_string(+10 * V_ions),             // maximum captured velocity [in units of c]
      to_string(20 * V_ions / 500.), to_string(20 * V_ions / 500.)  // step between nearest velocities [in units of c]
    }},
  }},
#endif

#if there_are_plasma_electrons
  { "plasma_electrons", {
    { "parameters", {
        to_string(n0),
        to_string(-e),
        to_string(me),
        to_string(Npi),
        to_string(T_electrons),
        to_string(T_electrons),
        "0",
        "0"
    }},
    { "integration_steps", {
        "Boris_pusher",
        "Simple_interpolation",
        "Esirkepov_density_decomposition",
    }},
    // Diagnostics with their config parameters
    { "energy", { "empty description" }},
    { "density", {
      "0", "0",
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),
      to_string(dx), to_string(dy),
    }},
    { "Vy_moment", {
      "0", "0",
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),
      to_string(dx), to_string(dy),
    }},
    { "mVxVx_moment", {
      "0", "0",
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),
      to_string(dx), to_string(dy),
    }},
    { "x0_distribution_function", {
      to_string(SIZE_X / 2),
      to_string(-10 * V_ions), to_string(-10 * V_ions),
      to_string(+10 * V_ions), to_string(+10 * V_ions),
      to_string(20 * V_ions / 500.), to_string(20 * V_ions / 500.)
    }},
  }},
#endif

#endif
};

inline const umap<std::string, std::vector<std::string>> fields_diagnostics = {
#if there_are_fields && fields_are_diagnosed
  { "energy", { "empty description" }},

  { "whole_field", { "J", "y", "0", "0", to_string(SIZE_X), to_string(SIZE_Y) }},
  { "whole_field", { "E", "x", "0", "0", to_string(SIZE_X), to_string(SIZE_Y) }},
  { "whole_field", { "E", "y", "0", "0", to_string(SIZE_X), to_string(SIZE_Y) }},
  { "whole_field", { "B", "z", "0", "0", to_string(SIZE_X), to_string(SIZE_Y) }},

  { "field_on_segment", { "E", "x", to_string(SIZE_X / 2), "0", to_string(SIZE_X / 2), to_string(SIZE_Y) }},
  { "field_on_segment", { "E", "y", to_string(SIZE_X / 2), "0", to_string(SIZE_X / 2), to_string(SIZE_Y) }},
  { "field_on_segment", { "B", "z", to_string(SIZE_X / 2), "0", to_string(SIZE_X / 2), to_string(SIZE_Y) }},

  { "field_on_segment", { "E", "x", to_string(SIZE_X / 2 + 100), "0", to_string(SIZE_X / 2 + 100), to_string(SIZE_Y) }},
  { "field_on_segment", { "E", "y", to_string(SIZE_X / 2 + 100), "0", to_string(SIZE_X / 2 + 100), to_string(SIZE_Y) }},
  { "field_on_segment", { "B", "z", to_string(SIZE_X / 2 + 100), "0", to_string(SIZE_X / 2 + 100), to_string(SIZE_Y) }},

  { "field_on_segment", { "E", "x", to_string(SIZE_X / 2 + 200), "0", to_string(SIZE_X / 2 + 200), to_string(SIZE_Y) }},
  { "field_on_segment", { "E", "y", to_string(SIZE_X / 2 + 200), "0", to_string(SIZE_X / 2 + 200), to_string(SIZE_Y) }},
  { "field_on_segment", { "B", "z", to_string(SIZE_X / 2 + 200), "0", to_string(SIZE_X / 2 + 200), to_string(SIZE_Y) }},

  { "field_on_segment", { "E", "x", to_string(SIZE_X / 2 + 300), "0", to_string(SIZE_X / 2 + 300), to_string(SIZE_Y) }},
  { "field_on_segment", { "E", "y", to_string(SIZE_X / 2 + 300), "0", to_string(SIZE_X / 2 + 300), to_string(SIZE_Y) }},
  { "field_on_segment", { "B", "z", to_string(SIZE_X / 2 + 300), "0", to_string(SIZE_X / 2 + 300), to_string(SIZE_Y) }},

#endif
};

}  // namespace config

#endif  // SRC_CONSTANTS_H
