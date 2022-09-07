#ifndef SRC_PARTICLES_SORT_COMPONENTS_HPP
#define SRC_PARTICLES_SORT_COMPONENTS_HPP

#include "src/pch.h"
#include "src/vectors/vector_classes.hpp"

struct Coordinate2D {
  std::vector<double> x_;
  std::vector<double> y_;

  vector2 operator[](size_t i) const {
    return {x_[i], y_[i]};
  }

  double& x(size_t i) { return this->x_[i]; }
  double& y(size_t i) { return this->y_[i]; }

  double x(size_t i) const { return this->x_[i]; }
  double y(size_t i) const { return this->y_[i]; }
};

struct Momentum3D {
  std::vector<double> x_;
  std::vector<double> y_;
  std::vector<double> z_;

  vector3 operator[](size_t i) const {
    return {x_[i], y_[i], z_[i]};
  }

  double& px(size_t i) { return this->x_[i]; }
  double& py(size_t i) { return this->y_[i]; }
  double& pz(size_t i) { return this->z_[i]; }

  double px(size_t i) const { return this->x_[i]; }
  double py(size_t i) const { return this->y_[i]; }
  double pz(size_t i) const { return this->z_[i]; }

  double get_gamma(size_t i, double mass) const {
    double pp_dot = x_[i] * x_[i] + y_[i] * y_[i] + z_[i] * z_[i];
    return sqrt(1 + pp_dot / (mass * mass));
  }

  double get_energy(size_t i, double mass) const {
    return get_gamma(i, mass) * mass;
  }

  double get_vx(size_t i, double mass) const {
    return px(i) / get_energy(i, mass);
  }

  double get_vy(size_t i, double mass) const {
    return py(i) / get_energy(i, mass);
  }

  double get_vz(size_t i, double mass) const {
    return pz(i) / get_energy(i, mass);
  }
};

#endif  // SRC_PARTICLES_SORT_COMPONENTS_HPP
