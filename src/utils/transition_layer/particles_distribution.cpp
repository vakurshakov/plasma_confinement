#include "particles_distribution.hpp"

#include "src/pch.h"
#include "src/particles/particles_load.hpp"
#include "src/utils/random_number_generator.hpp"
#include "src/utils/transition_layer/table_function.hpp"

namespace transition_layer {

static auto __n = Table_function("src/utils/transition_layer/n_" + config::postfix);
static auto __Gx = Table_function("src/utils/transition_layer/Gx_" + config::postfix);

void Random_coordinate_generator::load(double* x, double* y) {
  do {
    *x = config::domain_left + random_01() *
      (__n.get_xmax() - config::domain_left);
  }
  while (random_01() > get_probability(*x));

  *y = random_01() * SIZE_Y * dy;
}

double Random_coordinate_generator::get_probability(double x) const {
  if (x <= __n.get_x0()) {
    return 1.0;
  }
  else if (x <= __n.get_xmax()) {
    return __n(x);
  }
  else {
    return 0.0;
  }
}

int Random_coordinate_generator::get_particles_number() const {
  double integral = 0;

  for (double x = config::domain_left; x <= __n.get_xmax(); x += dx) {
    integral += get_probability(x);
  }

  return int(round(integral * SIZE_Y * config::Npi));
}


void load_maxwellian_impulse(double x, double y,
    double mass, double Tx, double Ty, double Tz,
    double p0, double* px, double* py, double* pz) {
  if (x <= __Gx.get_x0()) {
    double p = temperature_impulse(Tx, mass);  // Tx = Ty!
    double theta = 2 * M_PI * random_01();

    *px = p * cos(theta);
    *py = p * sin(theta);
  }
  else if (x <= __Gx.get_xmax()) {
    double p, e, theta;
    do {
      p = temperature_impulse(Tx, mass);  // Tx = Ty!
      e = sqrt(p * p + mass * mass);
      theta = - M_PI - asin(1.0 - __Gx(x) * e / p) + random_01() * (M_PI + 2 * asin(1.0 - __Gx(x) * e / p));
    }
    while (isnan(theta));

    *px = p * cos(theta);
    *py = p * sin(theta);
  }

  *pz = sin(2.0 * M_PI * random_01()) * temperature_impulse(Tz, mass);
}

}  // namespace transition_layer
