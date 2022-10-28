#ifndef SRC_CONSTANTS_H
#define SRC_CONSTANTS_H

#include <cmath>

#include <string>
#include <vector>
#include <unordered_map>

#define there_are_particles             true
  #define particles_are_diagnosed       true
  #define there_are_plasma_ions         true
  #define there_are_plasma_electrons    false

#define there_are_fields                true
  #define fields_are_diagnosed          true
  #define there_are_Bz0                 true

#define LOGGING                         true
#define TIME_PROFILING                  true

inline const double e   = 1.0;
inline const double me  = 1.0;
inline const double Mp  = 1836.0;

inline const std::string dir_name = "./results/test_dir_name";
inline const int NUM_THREADS = 1;

inline const double dx  = 0.05;
inline const int SIZE_X = 1460;

inline const double dy  = 0.05;
inline const int SIZE_Y = 800;

inline const double dt = 0.5 * dx;
inline const int TIME  = 1;

inline const int diagnose_time_step = 1;

namespace config {

inline const std::string boundaries = "cx_py";

inline const double Omega_max = 0.5;

inline const double mi_me  = 16.0;

// Ions velocity must satisfy the transverse
// equilibrium condition: ½ mᵢn₀V² = Bᵥ²/8π
inline const double V_ions = Omega_max / sqrt(mi_me);

inline const char* path_to_parameter_function =
  "src/utils/transition_layer/evaluated_function_20.0X0_0.05DX_16.0mi_me.bin";

inline const double n0 = 1.0;
inline const int   Npe = 1;
inline const double ni = 1.0;
inline const int   Npi = 100;

template<class K, class V>
using umap = std::unordered_multimap<K, V>;
using std::to_string;

inline const umap<std::string,
  umap<std::string, std::vector<std::string>>> species_description = {
#if there_are_particles

#if there_are_plasma_ions
  { "plasma_ions", {
    { "parameters", {
        "global, " + to_string(n0),     // Particle density [in units of n₀]
        "global, " + to_string(+e),     // Particle charge [in units of e]
        to_string(mi_me),               // Particle mass [in units mₑ]
        to_string(Npi),                 // Number of particles representing the density n₀
        "0", "0", "0",                  // Temperature in x, y and z direction [in KeV]
        to_string(mi_me * V_ions /      // Absolute value of the initial impulse [in mₑc]
          sqrt(1.0 - V_ions * V_ions))
    }},
    { "integration_steps", {
        "Boris_pusher:+Push_particle",
        "Boris_pusher:+Interpolation;",
        "Esirkepov_density_decomposition",
    }},
    // Diagnostics with their config parameters
    { "energy",          { "empty description" }},
    { "density",         { "0", "0", to_string(SIZE_X * dx), to_string(SIZE_Y * dy), "0.05", "0.05" }},
    { "first_Vx_moment", { "0", "0", to_string(SIZE_X * dx), to_string(SIZE_Y * dy), "0.05", "0.05" }},
    { "first_Vy_moment", { "0", "0", to_string(SIZE_X * dx), to_string(SIZE_Y * dy), "0.05", "0.05" }},
  }},
#endif

#if there_are_plasma_electrons
  { "plasma_electrons", {
    { "parameters", {
        "global, " + to_string(n0),
        "global, " + to_string(-e),
        to_string(me),
        to_string(Npe),
        "0", "0", "0",
        "0"
    }},
    { "integration_steps", {
        "Boris_pusher:+Push_particle",
        "Boris_pusher:+Interpolation;",
        "Esirkepov_density_decomposition",
    }},
    // Diagnostics with their config parameters
    { "density", { "0", "0", "200", "40", "0.05", "0.05" }},
  }},
#endif

#endif
};

inline const int damping_layer_width = 30;
inline const double damping_factor = 0.8;

// Domain_geometry
inline const double domain_left   = damping_layer_width * dx;
inline const double domain_right  = (SIZE_X - damping_layer_width) * dx;
inline const double domain_bottom = 0;
inline const double domain_top    = SIZE_Y * dy;

inline const umap<std::string, std::vector<std::string>> fields_diagnostics = {
#if there_are_fields && fields_are_diagnosed
  { "energy",      { "empty description" }},
  { "whole_field", { "E", "x", "0", "0", to_string(SIZE_X), to_string(SIZE_Y) }},
  { "whole_field", { "E", "y", "0", "0", to_string(SIZE_X), to_string(SIZE_Y) }},
  { "whole_field", { "B", "z", "0", "0", to_string(SIZE_X), to_string(SIZE_Y) }},
#endif
};

}  // namespace config

#endif  // SRC_CONSTANTS_H
