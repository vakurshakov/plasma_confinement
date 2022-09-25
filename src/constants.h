#ifndef SRC_CONSTANTS_H
#define SRC_CONSTANTS_H

#include <cmath>

#include <string>
#include <vector>
#include <unordered_map>

#define there_are_particles             true
  #define particles_are_diagnosed       true
  #define there_are_plasma_electrons    true
  #define there_are_plasma_ions         false
  #define there_are_beam_electrons      false
  #define there_are_beam_ions           false
  #define density_beam_profile_is_set   false

#define there_are_fields                true
  #define fields_are_diagnosed          true
  #define there_are_Bz0                 false

inline const double e   = 1.0;
inline const double me  = 1.0;
inline const double Mp  = 1836.0;

inline const std::string dir_name = "./results/test_dir_name";
inline const int NUM_THREADS = 2;

inline const double dx  = 0.05;
inline const double dy  = 0.05;
inline const int SIZE_X = 200;
inline const int SIZE_Y = 200;

inline const double dt = 0.5 * dx;
inline const int TIME  = 10'000;

inline const int diagnose_time_step = 5;

namespace config {

inline const std::string boundaries = "rx_ry";
inline const double Bz0 = 0.8;

inline const double n0 = 1.0;
inline const int Npe   = 1;
inline const double ni = 10.0;
inline const int Npi   = 1;

// Domain_geometry
inline const double domain_left   = 40 * dx;
inline const double domain_bottom = 40 * dy;
inline const double domain_right  = (SIZE_X - 40) * dx;
inline const double domain_top    = (SIZE_Y - 40) * dy;

template<class K, class V>
using umap = std::unordered_multimap<K, V>;
using std::to_string;

inline const umap<std::string,
  umap<std::string, std::vector<std::string>>> species_description = {
#if there_are_particles

#if there_are_plasma_electrons
  { "plasma_electrons", {
    { "parameters", {
        "global, " + to_string(n0),  // Particle density [in units of reference density]
        "global, " + to_string(-e),  // Particle charge [in units of e]
        to_string(me),               // Particle mass [in units mₑ]
        to_string(Npe),              // Number of particles representing the density n0
        "0", "0", "0",               // Temperature in x, y and z direction [in KeV]
        "0"                          // Absolute value of the initial impulse [in mₑc]
    }},
    { "integration_steps", {
        "Boris_pusher:+Push_particle",
        "Boris_pusher:+Interpolation;",
        "Esirkepov_density_decomposition",
    }},
    // Diagnostics with their config parameters
    { "density", { "0", "0", "10", "10", "0.05", "0.05" }},
  }},
#endif

#endif
};

inline const int damping_layer_width = 40;
inline const double damping_factor = 0.8;

inline const umap<std::string, std::vector<std::string>> fields_diagnostics = {
#if there_are_fields && fields_are_diagnosed
  { "whole_field", { "E", "x", "0", "0", "200", "200" }},
  { "whole_field", { "E", "y", "0", "0", "200", "200" }},
  { "whole_field", { "B", "z", "0", "0", "200", "200" }},
#endif
};

}  // namespace config

#endif  // SRC_CONSTANTS_H
