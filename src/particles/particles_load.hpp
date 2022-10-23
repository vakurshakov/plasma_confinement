#ifndef SRC_PARTICLES_PARTICLES_LOAD_HPP
#define SRC_PARTICLES_PARTICLES_LOAD_HPP

/// @todo impulse loader?

double temperature_impulse(double temperature, double mass);

void load_uniform_impulse(double x, double y,
  double mass, double Tx, double Ty, double Tz,
  double p0, double* px, double* py, double* pz);

#endif  // SRC_PARTICLES_PARTICLES_LOAD_HPP
