#ifndef SRC_CONSTANTS_H
#define SRC_CONSTANTS_H

#define THERE_ARE_PARTICLES             true
#define THERE_ARE_FIELDS                true

#define GLOBAL_DENSITY                  true

#define _2D3V                           false
#define LOGGING                         true
#define TIME_PROFILING                  true
#define MAKE_BACKUPS                    true
#define START_FROM_BACKUP               false

#define OMP_NUM_THREADS                 16

// Global geometry variables declaration
extern double dx;
extern int SIZE_X;

extern double dy;
extern int SIZE_Y;

extern double dt;
extern int TIME;

extern int diagnose_time_step;

#endif  // SRC_CONSTANTS_H
