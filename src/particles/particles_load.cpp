#include "particles_load.hpp"

#include "src/utils/random_number_generator.hpp"

#define TO_CELL(c_wp, ds) static_cast<int>(floor(c_wp / ds))

Fill_rectangle::Fill_rectangle(
  double x_min, double x_max,
  double y_min, double y_max)
  : x_min_(TO_CELL(x_min, dx)), x_max_(TO_CELL(x_max, dx)),
    y_min_(TO_CELL(y_min, dy)), y_max_(TO_CELL(y_max, dy)) {}

#undef TO_CELL

bool Fill_rectangle::operator()(int nx, int ny) {
  return
    x_min_ <= nx && nx <= x_max_ &&
    y_min_ <= ny && ny <= y_max_;
}

Fill_annulus::Fill_annulus(
  double radius,
  double center_x,
  double center_y)
  : inner_radius_(0),
    outer_radius_(radius),
    center_x_(center_x),
    center_y_(center_y) {}

Fill_annulus::Fill_annulus(
  double inner_radius,
  double outer_radius,
  double center_x,
  double center_y)
  : inner_radius_(inner_radius),
    outer_radius_(outer_radius),
    center_x_(center_x),
    center_y_(center_y) {}

bool Fill_annulus::operator()(int nx, int ny) {
  double x = nx * dx - center_x_;
  double y = ny * dy - center_y_;
  double rr = x * x + y * y;
  return
    rr <= outer_radius_ * outer_radius_ &&
    rr >= inner_radius_ * inner_radius_;
}


double uniform_profile(int nx, int ny) {
  return 1.0;
}


Coordinate_on_rectangle::Coordinate_on_rectangle(
  double x_min, double x_max,
  double y_min, double y_max)
  : x_min_(x_min), x_max_(x_min),
    y_min_(y_min), y_max_(y_min) {}

void Coordinate_on_rectangle::operator()(double& x, double& y) {
  x = x_min_ + random_01() * (x_max_ - x_min_);
  y = y_min_ + random_01() * (y_max_ - y_min_);
}

Coordinate_on_circle::Coordinate_on_circle(
  double radius,
  double center_x,
  double center_y)
  : radius_(radius),
    center_x_(center_x),
    center_y_(center_y) {}

void Coordinate_on_circle::operator()(double& x, double& y) {
  double r = radius_ * sqrt(random_01());
  double phi = 2.0 * M_PI * random_01();
  x = center_x_ + r * cos(phi);
  y = center_y_ + r * sin(phi);
}

Coordinate_on_annulus::Coordinate_on_annulus(
  double inner_radius,
  double outer_radius,
  double center_x,
  double center_y)
  : inner_radius_squared_(inner_radius * inner_radius),
    outer_radius_squared_(outer_radius * outer_radius),
    center_x_(center_x),
    center_y_(center_y) {}

void Coordinate_on_annulus::operator()(double& x, double& y) {
  double r = sqrt(inner_radius_squared_ + (outer_radius_squared_ - inner_radius_squared_) * random_01());
  double phi = 2.0 * M_PI * random_01();
  x = center_x_ + r * cos(phi);
  y = center_y_ + r * sin(phi);
}


Cosine_r_profile::Cosine_r_profile(
  double cos_width,
  double cos_center,
  double center_x,
  double center_y)
  : cos_width_(cos_width),
    cos_center_(cos_center),
    center_x_(center_x),
    center_y_(center_y) {}

double Cosine_r_profile::operator()(int nx, int ny) {
  double x = nx * dx - center_x_;
  double y = ny * dy - center_y_;
  double r = sqrt(x * x + y * y);

  if (fabs(r - cos_center_) >= cos_width_) {
    return 0.0;
  }
  return 0.5 * (1.0 + cos(2.0 * M_PI * (r - cos_center_) / cos_width_));
}


vector2 load_randomly(int nx, int ny) {
  return {
    (nx + random_01()) * dx,
    (ny + random_01()) * dy
  };
}


double temperature_momentum(double temperature, double mass) {
  static const double mec2 = 511.0;  // KeV

  double p = 0.0;
  do {
    p = sqrt(-2.0 * (temperature * mass / mec2) * log(random_01()));
  }
  while (std::isinf(p));

  return p;
}

Load_maxwellian_momentum::Load_maxwellian_momentum(const Parameters& parameters)
  : mass_(parameters.m()),
    Tx_(parameters.Tx()),
    Ty_(parameters.Ty()),
    Tz_(parameters.Tz()) {}

vector3 Load_maxwellian_momentum::operator()(const vector2& coordinate) {
  double px, py, pz;
  px = sin(2.0 * M_PI * random_01()) * temperature_momentum(Tx_, mass_);
  py = sin(2.0 * M_PI * random_01()) * temperature_momentum(Ty_, mass_);
#if _2D3V
  pz = sin(2.0 * M_PI * random_01()) * temperature_momentum(Tz_, mass_);
#else  // _2D2V
  pz = 0.0;
#endif
  return {px, py, pz};
}

Load_angular_momentum::Load_angular_momentum(
  const Parameters& parameters,
  double center_x,
  double center_y)
  : mass_(parameters.m()),
    Tx_(parameters.Tx()),
    Ty_(parameters.Ty()),
    Tz_(parameters.Tz()),
    p0_(parameters.p0()),
    center_x_(center_x),
    center_y_(center_y) {}

vector3 Load_angular_momentum::operator()(const vector2& coordinate) {
  double x = coordinate.x() - center_x_;
  double y = coordinate.y() - center_y_;
  double r = sqrt(x * x + y * y);

  double px, py, pz;
  px = -p0_ * (y / r) + temperature_momentum(Tx_, mass_);
  py = +p0_ * (x / r) + temperature_momentum(Ty_, mass_);

#if _2D3V
  pz = temperature_momentum(Tz_, mass_);
#else  // _2D2V
  pz = 0.0;
#endif
  return {px, py, pz};
}
