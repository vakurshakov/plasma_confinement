#include "particles_distribution.hpp"

#include "src/pch.h"
#include "src/particles/particles_load.hpp"
#include "src/utils/random_number_generator.hpp"
#include "src/utils/transition_layer/table_function.hpp"
#include "src/utils/transition_layer/parameters_table.hpp"

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

// __ZIGGURAT_ALGORITHM__
static auto __param = Parameters_table("src/utils/transition_layer/ziggurat_parameters.dat");

double maxwellian_distribution(double v) {
  const double v_therm_2 = config::V_ions * config::V_ions;

  return /* f(v) = */ exp(- 0.5 * v * v / v_therm_2) / (2.0 * M_PI * v_therm_2);
}

double inverse_maxwellian(double f) {
  const double v_therm_2 = config::V_ions * config::V_ions;

  return /* v(f) = */ sqrt(- 2.0 * v_therm_2 * log(2.0 * M_PI * v_therm_2 * f));
}

void ziggurat_table_generation() {
  const double v_therm = config::V_ions;
  const double v_therm_2 = v_therm * v_therm;
  const int max_n = 256;

  double v_prev = 3.9235487686865955 * v_therm;
  double z_prev = maxwellian_distribution(v_prev);

  const double A = M_PI * v_prev * v_prev * z_prev + exp(- 0.5 * v_prev * v_prev / v_therm_2);

  __param["v_0"] = sqrt(A / (M_PI * z_prev));
  __param["z_0"] = z_prev;

  __param["v_1"] = v_prev;
  __param["z_1"] = z_prev;

  for (int i = 2; i <= max_n; ++i) {
    z_prev = z_prev + A / (M_PI * v_prev * v_prev);
    v_prev = inverse_maxwellian(z_prev);

    __param["v_" + std::to_string(i)] = v_prev;
    __param["z_" + std::to_string(i)] = z_prev;
  }
  assert(abs(z_prev - maxwellian_distribution(0.0)) < 1e-10);

  __param["v_" + std::to_string(max_n)] = 0.0;
  __param["z_" + std::to_string(max_n)] = maxwellian_distribution(0.0);
  __param.rewrite();
}

void ziggurat_velocity_algorithm(double x, double& v, double& vx, double& vy) {
  static std::uniform_int_distribution random_level(0, 255);
  const double v_therm_2 = config::V_ions * config::V_ions;
  const double Gx = __Gx(x);

  while (true) {
    int i = random_level(Random_generator::get());
    double v_i = __param["v_" + std::to_string(i)];

    if (v_i < 0.5 * Gx) {
      continue;
    }

    v = sqrt(random_01()) * v_i;
    double theta = 2.0 * M_PI * random_01();

    if (v - (v * sin(theta) + Gx) < 0.0) {
      continue;
    }

    if (v < __param["v_" + std::to_string(i + 1)]) {
      vx = v * cos(theta);
      vy = v * sin(theta);
      return;
    }

    if (i == 0) {
      do {
        v = sqrt(v_i * v_i - 2.0 * v_therm_2 * log(random_01()));
      }
      while (random_01() * v >= v_i);

      vx = v * cos(theta);
      vy = v * sin(theta);
      return;
    }

    double z_curr = __param["z_" + std::to_string(i)];
    double z_next = __param["z_" + std::to_string(i + 1)];
    double z = z_curr + random_01() * (z_next - z_curr);

    if (z < maxwellian_distribution(v)) {
      vx = v * cos(theta);
      vy = v * sin(theta);
      return;
    }
  }
}

void load_maxwellian_impulse(double x, double y,
    double mass, double Tx, double Ty, double Tz,
    double p0, double* px, double* py, double* pz) {
  if (x <= __Gx.get_x0()) {
    double v, vx, vy;
    ziggurat_velocity_algorithm(__Gx.get_x0(), v, vx, vy);

    *px = mass * vx / sqrt(1.0 - v * v);
    *py = mass * vy / sqrt(1.0 - v * v);
  }
  else if (x <= __Gx.get_xmax()) {
    double v, vx, vy;
    ziggurat_velocity_algorithm(x, v, vx, vy);

    *px = mass * vx / sqrt(1.0 - v * v);
    *py = mass * vy / sqrt(1.0 - v * v);
  }

#if _2D3V
  *pz = sin(2.0 * M_PI * random_01()) * temperature_impulse(Tz, mass);

#else  // _2D2V
  *pz = 0.0;

#endif
}

}  // namespace transition_layer
