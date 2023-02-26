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


#if _TOMBSTONE_

void load_maxwellian_impulse(double x, double y,
    double mass, double Tx, double Ty, double Tz,
    double p0, double* px, double* py, double* pz) {
  static std::gamma_distribution<double> distribution;

  if (x <= __Gx.get_x0()) {
    double v;
    double theta = 2.0 * M_PI * random_01();

    #pragma omp critical
    v = config::V_ions * sqrt(2 * distribution(::Random_generator::get()));

    *px = 1.003 * mass * v * cos(theta);
    *py = 1.003 * mass * v * sin(theta);
  }
  else if (x <= __Gx.get_xmax()) {
    double Gx = __Gx(x);
    double v;
    do {
      #pragma omp critical
      v = config::V_ions * sqrt(2 * distribution(::Random_generator::get()));  // Tx = Ty!
    }
    while (v < 0.5 * Gx);

    double theta_max = asin(1.0 - Gx / v);
    double theta = -M_PI_2 + 2.0 * (random_01() - 0.5) * (M_PI_2 + theta_max);

    double cos_theta = cos(theta);
    double sin_theta = sin(theta);

    double vx_0 = v * cos_theta;
    double vy_0 = v * sin_theta;

    /// @tombstone: Don't ask me ever again...
    double& qx = *px;
    double& qy = *py;

    double nx = x / dx - 1133;
    double s = 145;

    double d_vx = (cos_theta > 0 ? +1 : -1) * 2.3 *
      exp(-0.5 * nx * nx / (s * s)) / sqrt(2 * M_PI * s * s);

    nx += 1133;

    qx = mass * (vx_0 + (nx < 1095 ? 1.0 : 1.09) * d_vx);
    qy = mass * (nx < 1090 ? 1.039 : 1.033) * vy_0;

    if (nx < 1025) { qx *= 1.0166; }
    if (nx > 1120) { qx *= 1.0020; }
    if (nx > 1180) { qx *= 1.0122; }
    if (nx > 1230) { qx *= 1.0210; }
    if (nx > 590 && nx < 700) { qx *= 0.9720; }
    if (nx > 659 && nx < 726) { qx *= 1.0010; }
    if (nx > 700 && nx < 726) { qx *= 0.9718; }
    if (nx > 600 && nx < 750) { qx *= 0.9880; }
    if (nx > 725 && nx < 810) { qx *= 0.9841; }
    if (nx > 810 && nx < 875) { qx *= 0.9885; }
    if (nx > 800 && nx < 915) { qx *= 1.0009; }
    if (nx > 900 && nx < 980) { qx *= 1.00548; }
    if (nx > 960 && nx <= 1050) { qx *= 1.0062; }
    if (nx > 990 && nx <= 1030) { qx *= 1.0050; }
    if (nx > 1000 && nx <= 1065) { qx *= 1.0057; }
    if (nx > 1020 && nx <= 1110) { qx *= 1.0169; }
    if (nx > 1030 && nx <= 1145) { qx *= 1.0081; }
    if (nx > 1121 && nx <= 1143) { qx *= 1.0090; }
    if (nx > 1121 && nx <= 1143) { qx *= 1.0074; }
    if (nx > 1141 && nx <= 1225) { qx *= 1.0210; }
    if (nx > 1160 && nx <= 1198) { qx *= 1.0163; }
    if (nx > 1198 && nx <= 1225) { qx *= 1.0267; }
    if (nx > 1210 && nx <= 1240) { qx *= 1.0081; }
    if (nx > 1225 && nx <= 1340) { qx *= 1.0573; }

    double q = sqrt(qx * qx + qy * qy);

    double phi_max = asin(1 - mass * Gx / q);
    double phi = -M_PI_2 + 2.0 * (random_01() - 0.5) * (M_PI_2 + phi_max);

    qx = q * cos(phi);
    qy = q * sin(phi);
  }

  *pz = sin(2.0 * M_PI * random_01()) * temperature_impulse(Tz, mass);
}

#else

void load_maxwellian_impulse(double x, double y,
    double mass, double Tx, double Ty, double Tz,
    double p0, double* px, double* py, double* pz) {
  if (x <= __Gx.get_x0()) {
    double p = temperature_impulse(Tx, mass);  // Tx = Ty!
    double theta = 2.0 * M_PI * random_01();

    *px = p * cos(theta);
    *py = p * sin(theta);
  }
  else if (x <= __Gx.get_xmax()) {
    double Gx = __Gx(x);
    double v;
    do {
      v = temperature_impulse(Tx, mass) / mass;  // Tx = Ty!
    }
    while (v < 0.5 * Gx);

    double theta_max = asin(1.0 - Gx / v);
    double theta = -M_PI_2 + 2.0 * (random_01() - 0.5) * (M_PI_2 + theta_max);

    *px = mass * v * cos(theta);
    *py = mass * v * sin(theta);
  }

  *pz = sin(2.0 * M_PI * random_01()) * temperature_impulse(Tz, mass);
}

#endif

}  // namespace transition_layer
