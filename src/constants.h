#ifndef SRC_CONSTANTS_H
#define SRC_CONSTANTS_H

//! @todo Refactor constants.h to contain only the global
//! constants other constants should be represented by some
//! name-accessible class (equivalent to map).

#include <math.h>

#include <string>
#include <vector>
#include <unordered_map>

#define there_are_particles             true
  #define GLOBAL_DENSITY                true
  #define particles_are_diagnosed       true
  #define there_are_plasma_ions         true
  #define there_are_plasma_electrons    true
  #define there_are_target_plasma       true

#define there_are_fields                true
  #define fields_are_diagnosed          true
  #define there_are_Bz0                 true

#define _2D3V                           false
#define LOGGING                         true
#define TIME_PROFILING                  false
#define MAKE_BACKUPS                    true
#define START_FROM_BACKUP               false
#define PARTICLES_FORM_FACTOR           2

// BOUNDARY_CONDITIONS:
#define NONE        -1
#define PEC         +0
#define PMC         +1
#define PERIODIC    +2
#define CONTINUOUS  +3

#define X_BOUNDARY_CONDITION            CONTINUOUS
#define Y_BOUNDARY_CONDITION            CONTINUOUS


#define AXIAL_INJECTION                 true


constexpr double e   = 1.0;
constexpr double me  = 1.0;
constexpr double Mp  = 1836.0;

inline const std::string dir_name = "./results/test_dir_name";

constexpr double dx = 0.025;
constexpr int SIZE_X = 4200;

constexpr double dy = dx;
constexpr int SIZE_Y = 4200;

constexpr double dt = 0.5 * dx;
constexpr int TIME  = 400'000;

static_assert(dx == dy, "Current setup works only with equal grid step!");
static_assert(SIZE_X == SIZE_Y, "Current setup works only with square grid!");

constexpr int diagnose_time_step = 200;
constexpr int backup_time_step = 10'000;

namespace config {

inline const double n0 = 1.0;
inline const int   Npi = 50;

inline const double mi_me = 16.0;

inline const double T_ions = 10.0;  // KeV
inline const double V_ions = sqrt(T_ions / mi_me / 511.0);

inline const double T_electrons = dx * dx * 511.0;  // KeV
inline const double V_electrons = sqrt(T_electrons / me / 511.0);

inline const double Omega_max = sqrt(2.0 * T_ions / 511.0);
inline const double ions_larmor_radius = mi_me * V_ions / Omega_max;

inline const int INJECTION_TIME = int(1000 / dt);

#if !AXIAL_INJECTION
inline const double R0 = 1.5 * ions_larmor_radius;
inline const int PER_STEP_PARTICLES = M_PI * R0 * R0 * Npi / (dx * dy * INJECTION_TIME);
#else
inline const double R0 = ions_larmor_radius;
inline const double DR = ions_larmor_radius;
inline const int PER_STEP_PARTICLES = 4.0 * M_PI * R0 * DR * Npi / (dx * dy * INJECTION_TIME);
#endif

#if there_are_target_plasma
inline const int INJECTION_START = int(1000 / dt);
inline const double TARGET_PLASMA_TEMPERATURE = dx * dx * 511.0;  // KeV
inline const double RADIUS_OF_TARGET_PLASMA = 2.0 * ions_larmor_radius;
#else
inline const int INJECTION_START = 0;
#endif

inline const double damping_layer_width = 100;
inline const double damping_factor = 0.7;

// Domain_geometry
inline const double domain_r_min   = 0.0;
inline const double domain_r_max   = (0.5 * SIZE_X - damping_layer_width) * dx;
inline const double domain_phi_min = 0.0;
inline const double domain_phi_max = 2.0 * M_PI;


template<class K, class V>
using umap = std::unordered_multimap<K, V>;
using std::to_string;

inline const umap<std::string,
  umap<std::string, std::vector<std::string>>> species_description = {
#if there_are_particles

#if there_are_plasma_ions
  { "plasma_ions", {
    { "parameters", {
        to_string(n0),       // Particle density [in units of n₀]
        to_string(+e),       // Particle charge [in units of e]
        to_string(mi_me),    // Particle mass [in units mₑ]
        to_string(Npi),      // Number of particles representing the density n₀
        "0.0",               // Temperature in x, y and z direction [in KeV]
        "0.0",               //
        "0.0",               //
        to_string(mi_me * V_ions / sqrt(1.0 - V_ions * V_ions))  // Absolute value of the initial impulse [in units of mₑc]
    }},
    { "integration_steps", {
        "Boris_pusher",
        "Simple_interpolation",
        "Esirkepov_density_decomposition",
    }},
    // Diagnostics with their config parameters
    { "energy", { "empty description" }},
    { "density", {                                       //
      "0", "0",                                          // minimal captured coordinate [in units of c/ωₚ]
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),    // maximum captured coordinate [in units of c/ωₚ]
      to_string(dx), to_string(dy),                      // step between nearest coordinates [in units of c/ωₚ]
    }},
    { "Vphi_moment", {                                   //
      "0", "0",                                          // minimal captured coordinate [in units of c/ωₚ]
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),    // maximum captured coordinate [in units of c/ωₚ]
      to_string(dx), to_string(dy),                      // step between nearest coordinates [in units of c/ωₚ]
    }},
    { "mVrVr_moment", {                                  //
      "0", "0",                                          // minimal captured coordinate [in units of c/ωₚ]
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),    // maximum captured coordinate [in units of c/ωₚ]
      to_string(dx), to_string(dy),                      // step between nearest coordinates [in units of c/ωₚ]
    }},
    { "mVphiVphi_moment", {                              //
      "0", "0",                                          // minimal captured coordinate [in units of c/ωₚ]
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),    // maximum captured coordinate [in units of c/ωₚ]
      to_string(dx), to_string(dy),                      // step between nearest coordinates [in units of c/ωₚ]
    }},
    { "VrVphi_distribution", {                                     //
      to_string(-10 * V_ions), to_string(-10 * V_ions),            // minimal captured velocity [in units of c]
      to_string(+10 * V_ions), to_string(+10 * V_ions),            // maximum captured velocity [in units of c]
      to_string(20 * V_ions / 500.), to_string(20 * V_ions / 500.),  // step between nearest velocities [in units of c]
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
        "0.0",
        "0.0",
        "0.0",
        to_string(me * V_ions / sqrt(1.0 - V_ions * V_ions))
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
    { "Vphi_moment", {
      "0", "0",
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),
      to_string(dx), to_string(dy),
    }},
    { "mVrVr_moment", {
      "0", "0",
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),
      to_string(dx), to_string(dy),
    }},
    { "mVphiVphi_moment", {
      "0", "0",
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),
      to_string(dx), to_string(dy),
    }},
    { "VrVphi_distribution", {
      to_string(-10 * V_ions), to_string(-10 * V_ions),
      to_string(+10 * V_ions), to_string(+10 * V_ions),
      to_string(20 * V_ions / 500.), to_string(20 * V_ions / 500.),
    }},
  }},
#endif

#if there_are_target_plasma
  { "target_ions", {
    { "parameters", {
        to_string(n0),
        to_string(+e),
        to_string(mi_me),
        to_string(Npi),
        to_string(TARGET_PLASMA_TEMPERATURE),
        to_string(TARGET_PLASMA_TEMPERATURE),
        "0.0",
        "0.0"
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
    { "Vphi_moment", {
      "0", "0",
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),
      to_string(dx), to_string(dy),
    }},
    { "mVrVr_moment", {
      "0", "0",
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),
      to_string(dx), to_string(dy),
    }},
    { "mVphiVphi_moment", {
      "0", "0",
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),
      to_string(dx), to_string(dy),
    }},
    { "VrVphi_distribution", {
      to_string(-10 * V_ions), to_string(-10 * V_ions),
      to_string(+10 * V_ions), to_string(+10 * V_ions),
      to_string(20 * V_ions / 500.), to_string(20 * V_ions / 500.),
    }},
  }},

  { "target_electrons", {
    { "parameters", {
        to_string(n0),
        to_string(-e),
        to_string(me),
        to_string(Npi),
        to_string(TARGET_PLASMA_TEMPERATURE),
        to_string(TARGET_PLASMA_TEMPERATURE),
        "0.0",
        "0.0"
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
    { "Vphi_moment", {
      "0", "0",
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),
      to_string(dx), to_string(dy),
    }},
    { "mVrVr_moment", {
      "0", "0",
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),
      to_string(dx), to_string(dy),
    }},
    { "mVphiVphi_moment", {
      "0", "0",
      to_string(SIZE_X * dx), to_string(SIZE_Y * dy),
      to_string(dx), to_string(dy),
    }},
    { "VrVphi_distribution", {
      to_string(-10 * V_ions), to_string(-10 * V_ions),
      to_string(+10 * V_ions), to_string(+10 * V_ions),
      to_string(20 * V_ions / 500.), to_string(20 * V_ions / 500.),
    }},
  }},
#endif

#endif
};

inline const umap<std::string, std::vector<std::string>> fields_diagnostics = {
#if there_are_fields && fields_are_diagnosed
  { "energy", { "empty description" }},

  { "whole_field", { "E", "x", "0", "0", to_string(SIZE_X), to_string(SIZE_Y) }},
  { "whole_field", { "E", "y", "0", "0", to_string(SIZE_X), to_string(SIZE_Y) }},
  { "whole_field", { "B", "z", "0", "0", to_string(SIZE_X), to_string(SIZE_Y) }},

#endif
};

}  // namespace config

#endif  // SRC_CONSTANTS_H
