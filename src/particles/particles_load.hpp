#ifndef SRC_PARTICLES_PARTICLES_LOAD_HPP
#define SRC_PARTICLES_PARTICLES_LOAD_HPP

#include "src/pch.h"
#include "src/particles/particle/parameters.hpp"
#include "src/vectors/vector_classes.hpp"

class Filling_condition {
 public:
  virtual ~Filling_condition() = default;
  virtual bool operator()(int nx, int ny) = 0;
};

class Fill_rectangle final : public Filling_condition {
 public:
  Fill_rectangle(double x_min, double x_max, double y_min, double y_max);
  bool operator()(int nx, int ny) override;

 private:
  int x_min_, x_max_, y_min_, y_max_;
};

class Fill_annulus final : public Filling_condition {
 public:
  Fill_annulus(double radius, double center_x, double center_y);
  Fill_annulus(double inner_radius, double outer_radius, double center_x, double center_y);
  bool operator()(int nx, int ny) override;

 private:
  double inner_radius_, outer_radius_;
  double center_x_, center_y_;
};


class Density_profile {
 public:
  virtual ~Density_profile() = default;
  virtual double operator()(int nx, int ny) = 0;
};

double uniform_profile(int nx, int ny);

class Cosine_r_profile final : Density_profile {
 public:
  Cosine_r_profile(double cos_width, double cos_center, double center_x, double center_y);
  double operator()(int nx, int ny) override;

 private:
  double cos_width_, cos_center_;
  double center_x_, center_y_;
};


vector2 load_randomly(int nx, int ny);


double temperature_momentum(double temperature, double mass);

class Moment_generator {
 public:
  virtual ~Moment_generator() = default;
  virtual vector3 operator()(const vector2& coordinate) = 0;
};

class Load_maxwellian_momentum final : public Moment_generator {
 public:
  Load_maxwellian_momentum(const Parameters& parameters);
  vector3 operator()(const vector2& coordinate) override;

 private:
  double mass_, Tx_, Ty_, Tz_;
};

class Load_angular_momentum final : public Moment_generator {
 public:
  Load_angular_momentum(const Parameters& parameters, double center_x, double center_y);
  vector3 operator()(const vector2& coordinate) override;

 private:
  double mass_, Tx_, Ty_, Tz_, p0_;
  double center_x_, center_y_;
};

using impulse_loader = std::function<
void(double x, double y,
  double mass, double Tx, double Ty, double Tz,
  double p0, double* px, double* py, double* pz)>;

#endif  // SRC_PARTICLES_PARTICLES_LOAD_HPP
