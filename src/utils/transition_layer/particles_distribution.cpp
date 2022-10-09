#include "particles_distribution.hpp"
#include "parameter_function.hpp"

#include "src/pch.h"
#include "src/utils/transition_layer/parameter_function.hpp" 
#include "src/managers/random_number_generator.hpp"

/// @todo Check it is thread-safe
static auto& __func = transition_layer::Parameter_function::instance();

namespace transition_layer {

double d_theta(double x) {
  return M_PI + 2 * asin(__func.get_value(x));
}


void Random_coordinate_generator::load(double* x, double* y) {
  do {
    *x = config::domain_left + random_01() *
      (__func.get_xmax() - config::domain_left);
    
    *y = random_01() * SIZE_Y * dx;
  }
  while (random_01() > get_probability(*x, *y));
}

double Random_coordinate_generator::get_probability(double x, double y) const {
  if (x <= __func.get_x0()) {
    return 1.0;
  }
  else if (x < __func.get_xmax()) {
    return d_theta(x) / (2 * M_PI);
  }
  else {
    return 0.0;
  }
}

int Random_coordinate_generator::get_particles_number() const {
  double integral = 0;

  for (double x = config::domain_left; x < __func.get_xmax(); x += dx) {
    integral += get_probability(x, 0);
  }

  return int(round(integral * SIZE_Y * config::Npi));
}

void load_ions_impulse(double x, double y,
	  double mass, double Tx, double Ty, double Tz,
	  double p0, double* px, double* py, double* pz) {
  double theta = 0.0;
  if (x <= __func.get_x0()) {
    theta = 2 * M_PI * random_01(); 
  }
  else if (x < __func.get_xmax()) {
    theta = - M_PI - asin(__func.get_value(x)) + random_01() * d_theta(x);
  }

  *px = config::mi_me * config::V_ions * cos(theta);
  *py = config::mi_me * config::V_ions * sin(theta);
  *pz = 0.0;
}

}  // namespace transition_layer
