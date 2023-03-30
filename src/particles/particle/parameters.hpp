#ifndef GLOBAL_PARAMETERS_HPP
#define GLOBAL_PARAMETERS_HPP

#include "src/pch.h"

double zeroth_order_spline(double x, double grid_mesh);
double first_order_spline(double x, double grid_mesh);
double second_order_spline(double x, double grid_mesh);
double third_order_spline(double x, double grid_mesh);
double fourth_order_spline(double x, double grid_mesh);
double fifth_order_spline(double x, double grid_mesh);


struct Parameters {
  int Np_;               // Number of particles in a cell.
  double n_;             // Reference density of the particles.
  double q_;             // Reference charge of the particles.
  double m_;             // Mass of the particles in a sort.
  double p0_;            // Initial impulse (absolute value).
  double Tx_, Ty_, Tz_;  // Temperature in x, y and z directions.

  int charge_cloud_ = 2;
  std::function<double(double, double)> form_factor_ = second_order_spline;

  std::string sort_name_;

  Parameters() = default;
  ~Parameters() = default;

  constexpr double n() const { return n_; }
  constexpr double q() const { return q_; }
  constexpr double m() const { return m_; }

  constexpr int Np() const { return Np_; }
  constexpr double p0() const { return p0_; }
  constexpr double Tx() const { return Tx_; }
  constexpr double Ty() const { return Ty_; }
  constexpr double Tz() const { return Tz_; }

  constexpr int charge_cloud() const { return charge_cloud_; }
  constexpr const auto& form_factor() const { return form_factor_; }

  constexpr const std::string& get_name() const { return sort_name_; }
};

#endif //GLOBAL_PARAMETERS_HPP
