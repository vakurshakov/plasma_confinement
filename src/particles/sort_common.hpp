#ifndef SRC_PARTICLES_SORT_COMMON_HPP
#define SRC_PARTICLES_SORT_COMMON_HPP

#include "src/pch.h"

struct spline_shape {
  virtual ~spline_shape() = default;
  spline_shape(int width);

  const int width;
  virtual double form(double x, double dx) const = 0;
};

struct Common_parameters {
  Common_parameters() = default;
  Common_parameters(Common_parameters&&) = default;

  int Np;             // Number of particles in a cell.
  double m;           // Mass of the particles in a sort.
  double n;           // Initial uniform density of the particles.
  double q;           // Initial uniform charge of the particles.
  double p0;          // Initial impulse (absolute value).
	double Tx, Ty, Tz;  // Temperature in x, y and z directions
  
  using spline_shape_up = std::unique_ptr<spline_shape>;
  spline_shape_up shape;
};

#endif  // SRC_PARTICLES_SORT_COMMON_HPP
