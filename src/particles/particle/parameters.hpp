#ifndef GLOBAL_PARAMETERS_HPP
#define GLOBAL_PARAMETERS_HPP

#include "src/pch.h"

struct Parameters {
  Parameters() = default;
  ~Parameters() = default;

  int Np_;               // Number of particles in a cell.
  double n_;             // Reference density of the particles.
  double q_;             // Reference charge of the particles.
  double m_;             // Mass of the particles in a sort.
  double p0_;            // Initial impulse (absolute value).
  double Tx_, Ty_, Tz_;  // Temperature in x, y and z directions.

  constexpr int Np() const { return Np_; }
  constexpr double n() const { return n_; }
  constexpr double q() const { return q_; }
  constexpr double m() const { return m_; }
  constexpr double p0() const { return p0_; }
  constexpr double Tx() const { return Tx_; }
  constexpr double Ty() const { return Ty_; }
  constexpr double Tz() const { return Tz_; }

  std::string sort_name_;
  constexpr const std::string& get_name() const { return sort_name_; }
};


double zeroth_order_spline(double x, double grid_mesh);
double first_order_spline(double x, double grid_mesh);
double second_order_spline(double x, double grid_mesh);
double third_order_spline(double x, double grid_mesh);
double fourth_order_spline(double x, double grid_mesh);
double fifth_order_spline(double x, double grid_mesh);

#if (PARTICLES_FORM_FACTOR == 0)
  static constexpr int shape_radius = 1;
  static constexpr const auto& shape_function = zeroth_order_spline;
#elif (PARTICLES_FORM_FACTOR == 1)
  static constexpr int shape_radius = 1;
  static constexpr const auto& shape_function = first_order_spline;
#elif (PARTICLES_FORM_FACTOR == 2)
  static constexpr int shape_radius = 2;
  static constexpr const auto& shape_function = second_order_spline;
#elif (PARTICLES_FORM_FACTOR == 3)
  static constexpr int shape_radius = 2;
  static constexpr const auto& shape_function = third_order_spline;
#elif (PARTICLES_FORM_FACTOR == 4)
  static constexpr int shape_radius = 3;
  static constexpr const auto& shape_function = fourth_order_spline;
#elif (PARTICLES_FORM_FACTOR == 5)
  static constexpr int shape_radius = 3;
  static constexpr const auto& shape_function = fifth_order_spline;
#endif

#endif //GLOBAL_PARAMETERS_HPP
