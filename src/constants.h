#ifndef SRC_CONSTANTS_H
#define SRC_CONSTANTS_H

#include <cmath>

#include <string>
#include <vector>
#include <unordered_map>

#define there_are_particles             true
  #define particles_are_diagnosed       true
  #define there_are_plasma_ions         true
  #define there_are_plasma_electrons    true
  #define IS_DENSITY_GLOBAL             true

#define there_are_fields                true
  #define fields_are_diagnosed          true
  #define there_are_Bz0                 true

#define LOGGING                         true
#define TIME_PROFILING                  true

#define BEAM_INJECTION_SETUP            false
#define PLASMA_MAG_FIELD_SETUP          true

inline const double e   = 1.0;
inline const double me  = 1.0;
inline const double Mp  = 1836.0;

inline const std::string dir_name = "./results/test_dir_name";
inline const int NUM_THREADS = 1;

inline const double dx  = 0.05;
inline const int SIZE_X = 2100;

inline const double dy  = 0.05;
inline const int SIZE_Y = 400;

inline const double dt = 0.5 * dx;
inline const int TIME  = 1;

inline const int diagnose_time_step = 1;

namespace config {

// This factor describes how many times the
// width of the copy layer is greater than the
// half-width of the one particle cloud
inline const double COPY_LAYER_MULT = 1;

inline const std::string boundaries = "cx_py";

inline const double n0 = 1.0;
inline const int   Npi = 50;

inline const double mi_me  = 16.0;

inline const double Omega_max = 0.2;

// Injection is done so that after INJECTION_TIME there would be n0
inline const int INJECTION_TIME = 40'000;
inline const int WIDTH_OF_INJECTION_AREA = 500;
inline const int PER_STEP_PARTICLES = SIZE_Y * WIDTH_OF_INJECTION_AREA * Npi / INJECTION_TIME;

// Ions temperature must satisfy the transverse
// equilibrium condition: n₀kT = Bᵥ²/8π
inline const double IONS_TEMPERATURE_MULT = 1.0;
inline const double T_ions = 0.5 * Omega_max * Omega_max / n0 * 511.0 * IONS_TEMPERATURE_MULT;  // [in KeV], ~10 KeV for mi = 16
inline const double V_ions = sqrt(2 * T_ions / mi_me / 511.0);

inline const double ELECTRONS_TEMPERATURE_MULT = 0.005;
inline const double T_electrons = ELECTRONS_TEMPERATURE_MULT * T_ions;  // ~50 eV for ETM = 0.005
inline const double V_electrons = sqrt(2 * T_electrons / me / 511.0);

// Constants describing damping layer and calculation domain
inline const int damping_layer_width = 50;
inline const double damping_factor = 0.9;

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
        to_string(T_ions),  //
        "0"                 // Absolute value of the initial impulse [in units of mₑc]
    }},
    { "integration_steps", {
        "Boris_pusher:+Push_particle",
        "Boris_pusher:+Interpolation;",
        "Esirkepov_density_decomposition",
    }},
    // Diagnostics with their config parameters
    { "energy", { "empty description" }},
    { "density", {                                                  //
      "0", "0",                                                     // minimal captured coordinate [in units of c/ωₚ]
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),               // maximum captured coordinate [in units of c/ωₚ]
      to_string(dx), to_string(dy),                                 // step between nearest coordinates [in units of c/ωₚ]
    }},
    { "first_Vx_moment", {                                          //
      "0", "0",                                                     // minimal captured coordinate [in units of c/ωₚ]
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),               // maximum captured coordinate [in units of c/ωₚ]
      to_string(dx), to_string(dy),                                 // step between nearest coordinates [in units of c/ωₚ]
    }},
    { "first_Vy_moment", {                                          //
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
    { "x0_distribution_function", {
      to_string((SIZE_X + WIDTH_OF_INJECTION_AREA) / 2),
      to_string(-10 * V_ions), to_string(-10 * V_ions),
      to_string(+10 * V_ions), to_string(+10 * V_ions),
      to_string(20 * V_ions / 500.), to_string(20 * V_ions / 500.)
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
        to_string(T_electrons),
        "0"
    }},
    { "integration_steps", {
        "Boris_pusher:+Push_particle",
        "Boris_pusher:+Interpolation;",
        "Esirkepov_density_decomposition",
    }},
    // Diagnostics with their config parameters
    { "energy", { "empty description" }},
    { "density", {
      "0", "0",
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),
      to_string(dx), to_string(dy),
    }},
    { "first_Vx_moment", {
      "0", "0",
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),
      to_string(dx), to_string(dy),
    }},
    { "first_Vy_moment", {
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
    { "x0_distribution_function", {
      to_string((SIZE_X + WIDTH_OF_INJECTION_AREA) / 2),
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

  { "whole_field", { "J", "x", "0", "0", to_string(SIZE_X), to_string(SIZE_Y) }},
  { "whole_field", { "J", "y", "0", "0", to_string(SIZE_X), to_string(SIZE_Y) }},
  { "whole_field", { "E", "x", "0", "0", to_string(SIZE_X), to_string(SIZE_Y) }},
  { "whole_field", { "E", "y", "0", "0", to_string(SIZE_X), to_string(SIZE_Y) }},
  { "whole_field", { "B", "z", "0", "0", to_string(SIZE_X), to_string(SIZE_Y) }},

  { "field_on_segment", {
    "E", "x",
    to_string(SIZE_X / 2), "0",
    to_string(SIZE_X / 2), to_string(SIZE_Y)
  }},
  { "field_on_segment", {
    "E", "y",
    to_string(SIZE_X / 2), "0",
    to_string(SIZE_X / 2), to_string(SIZE_Y)
  }},
  { "field_on_segment", {
    "B", "z",
    to_string(SIZE_X / 2), "0",
    to_string(SIZE_X / 2), to_string(SIZE_Y)
  }},

#if BEAM_INJECTION_SETUP
  { "field_on_segment", {
    "E", "x",
    to_string((SIZE_X + WIDTH_OF_INJECTION_AREA) / 2), "0",
    to_string((SIZE_X + WIDTH_OF_INJECTION_AREA) / 2), to_string(SIZE_Y)
  }},
  { "field_on_segment", {
    "E", "y",
    to_string((SIZE_X + WIDTH_OF_INJECTION_AREA) / 2), "0",
    to_string((SIZE_X + WIDTH_OF_INJECTION_AREA) / 2), to_string(SIZE_Y)
  }},
  { "field_on_segment", {
    "B", "z",
    to_string((SIZE_X + WIDTH_OF_INJECTION_AREA) / 2), "0",
    to_string((SIZE_X + WIDTH_OF_INJECTION_AREA) / 2), to_string(SIZE_Y)
  }},
#endif
#endif
};

}  // namespace config

#endif  // SRC_CONSTANTS_H
