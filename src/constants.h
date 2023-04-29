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
  #define there_are_target_plasma       true

#define there_are_fields                true
  #define fields_are_diagnosed          true
  #define there_are_Bz0                 true

#define _2D3V                           false
#define LOGGING                         true
#define TIME_PROFILING                  true
#define MAKE_BACKUPS                    true
#define START_FROM_BACKUP               false

#define OMP_NUM_THREADS                 16

constexpr inline double dx = 0.05;
constexpr inline int SIZE_X = 2100;

constexpr inline double dy = dx;
constexpr inline int SIZE_Y = 2100;

constexpr inline double dt = 0.5 * dx;
constexpr inline int TIME  = 400'000;

static_assert(dx == dy, "Current setup works only with equal grid step!");
static_assert(SIZE_X == SIZE_Y, "Current setup works only with square grid!");

constexpr inline int diagnose_time_step = 100;

#endif  // SRC_CONSTANTS_H
