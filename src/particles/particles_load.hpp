#ifndef SRC_PARTICLES_PARTICLES_LOAD_HPP
#define SRC_PARTICLES_PARTICLES_LOAD_HPP

#include "src/pch.h"

double temperature_impulse(double temperature, double mass);

void load_maxwellian_impulse(double x, double y,
  double mass, double Tx, double Ty, double Tz,
  double p0, double* px, double* py, double* pz);

using impulse_loader = std::function<
void(double x, double y,
  double mass, double Tx, double Ty, double Tz,
  double p0, double* px, double* py, double* pz)>;

#endif  // SRC_PARTICLES_PARTICLES_LOAD_HPP
