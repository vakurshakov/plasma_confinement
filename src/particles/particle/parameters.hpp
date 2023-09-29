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


#pragma omp declare simd uniform(ds), notinbranch
double __0th_order_spline(double s, double ds);

#pragma omp declare simd uniform(ds), notinbranch
double __1st_order_spline(double s, double ds);

#pragma omp declare simd uniform(ds), notinbranch
double __2nd_order_spline(double s, double ds);

#pragma omp declare simd uniform(ds), notinbranch
double __3rd_order_spline(double s, double ds);

#pragma omp declare simd uniform(ds), notinbranch
double __4th_order_spline(double s, double ds);

#pragma omp declare simd uniform(ds), notinbranch
double __5th_order_spline(double s, double ds);


#if (PARTICLES_FORM_FACTOR == 0)
  static constexpr int shape_radius = 1;
  static constexpr const auto& shape_function = __0th_order_spline;
#elif (PARTICLES_FORM_FACTOR == 1)
  static constexpr int shape_radius = 1;
  static constexpr const auto& shape_function = __1st_order_spline;
#elif (PARTICLES_FORM_FACTOR == 2)
  static constexpr int shape_radius = 2;
  static constexpr const auto& shape_function = __2nd_order_spline;
#elif (PARTICLES_FORM_FACTOR == 3)
  static constexpr int shape_radius = 2;
  static constexpr const auto& shape_function = __3rd_order_spline;
#elif (PARTICLES_FORM_FACTOR == 4)
  static constexpr int shape_radius = 3;
  static constexpr const auto& shape_function = __4th_order_spline;
#elif (PARTICLES_FORM_FACTOR == 5)
  static constexpr int shape_radius = 3;
  static constexpr const auto& shape_function = __5th_order_spline;
#else
  #error "Unknown PARTICLES_FORM_FACTOR is specified!"
#endif

#endif //GLOBAL_PARAMETERS_HPP
