#include "vector3_field.hpp"

// It doesn't include cases where begin > 0.
inline int periodic(int g, int begin, int end) {
  int pg = g % end;

  if (g < begin)
    pg += end;

  return pg;
}

inline int continuous(int g, int begin, int end) {
  if (begin <= g && g < end) {
    return g;
  }
  else if (g < begin) {
    return begin;
  }
  else {
    return end - 1;
  }
}

inline bool in_bounds(int g, int begin, int end) {
  return begin <= g && g < end;
}


vector3_field::vector3_field(int size_x, int size_y)
    : size_x_{size_x}, size_y_{size_y} {
  fx_.reserve(size_x_ * size_y_);
  fy_.reserve(size_x_ * size_y_);
  fz_.reserve(size_x_ * size_y_);
}

constexpr int vector3_field::index(int ny, int nx) const {
  return ny * size_x_ + nx;
}

int vector3_field::ix_first(Axis /* axis */) const { return 0; }
int vector3_field::iy_first(Axis /* axis */) const { return 0; }
int vector3_field::ix_last (Axis /* axis */) const { return size_x_; }
int vector3_field::iy_last (Axis /* axis */) const { return size_y_; }

double& vector3_field::x(int ny, int nx) {
  return fx_[index(ny, nx)];
}
double& vector3_field::y(int ny, int nx) {
  return fy_[index(ny, nx)];
}
double& vector3_field::z(int ny, int nx) {
  return fz_[index(ny, nx)];
}

double vector3_field::x(int ny, int nx) const {
  return fx_[index(ny, nx)];
}
double vector3_field::y(int ny, int nx) const {
  return fy_[index(ny, nx)];
}
double vector3_field::z(int ny, int nx) const {
  return fz_[index(ny, nx)];
}

vector3 vector3_field::operator()(int ny, int nx) const {
  return {
    x(ny, nx),
    y(ny, nx),
    z(ny, nx),
  };
}

double vector3_field::operator()(int ny, int nx, Axis comp) const {
  switch (comp) {
    case Axis::X: return x(ny, nx);
    case Axis::Y: return y(ny, nx);
    case Axis::Z: return z(ny, nx);
  }
  return 0.0;
}


px_py_vector3_field::px_py_vector3_field(int size_x, int size_y)
  : vector3_field(size_x, size_y) {}

int px_py_vector3_field::ix_first(Axis /* axis */) const { return 0; }
int px_py_vector3_field::iy_first(Axis /* axis */) const { return 0; }
int px_py_vector3_field::ix_last (Axis /* axis */) const { return size_x_; }
int px_py_vector3_field::iy_last (Axis /* axis */) const { return size_y_; }

double& px_py_vector3_field::x(int ny, int nx) {
  return fx_[index(periodic(ny, 0, size_y_), periodic(nx, 0, size_x_))];
}
double& px_py_vector3_field::y(int ny, int nx) {
  return fy_[index(periodic(ny, 0, size_y_), periodic(nx, 0, size_x_))];
}
double& px_py_vector3_field::z(int ny, int nx) {
  return fz_[index(periodic(ny, 0, size_y_), periodic(nx, 0, size_x_))];
}

double px_py_vector3_field::x(int ny, int nx) const {
  return fx_[index(periodic(ny, 0, size_y_), periodic(nx, 0, size_x_))];
}
double px_py_vector3_field::y(int ny, int nx) const {
  return fy_[index(periodic(ny, 0, size_y_), periodic(nx, 0, size_x_))];
}
double px_py_vector3_field::z(int ny, int nx) const {
  return fz_[index(periodic(ny, 0, size_y_), periodic(nx, 0, size_x_))];
}


rx_py_vector3_field::rx_py_vector3_field(std::string type, int size_x, int size_y)
    : vector3_field(size_x, size_y) {
  if ( type == "Electric" ) {
    bound_.left[X] = 0;  bound_.right[X] = size_x_;
    bound_.left[Y] = 1;  bound_.right[Y] = size_x_;
    bound_.left[Z] = 0;  bound_.right[Z] = size_x_ - 1;
  }
  else if (type == "Magnetic") {
    bound_.left[X] = 0;  bound_.right[X] = size_x_ - 1;
    bound_.left[Y] = 0;  bound_.right[Y] = size_x_;
    bound_.left[Z] = 0;  bound_.right[Z] = size_x_;
  }
}

int rx_py_vector3_field::ix_first(Axis axis) const { return bound_.left[axis];  }
int rx_py_vector3_field::ix_last (Axis axis) const { return bound_.right[axis]; }
int rx_py_vector3_field::iy_first(Axis axis) const { return 0; }
int rx_py_vector3_field::iy_last (Axis axis) const { return size_y_; }

inline double& rx_py_vector3_field::get_proxied(Axis axis, int ny, int nx) {
  return const_cast<double&>(std::as_const(*this).get_proxied(axis, ny, nx));
}

inline const double& rx_py_vector3_field::get_proxied(Axis axis, int ny, int nx) const {
  ny = periodic(ny, 0, size_y_);

  if (in_bounds(nx, bound_.left[axis], bound_.right[axis])) {
    switch (axis) {
      case Axis::X: return fx_[index(ny, nx)];
      case Axis::Y: return fy_[index(ny, nx)];
      case Axis::Z: return fz_[index(ny, nx)];
    }
  }

  zero_ = 0.0;
  return zero_;
}

double& rx_py_vector3_field::x(int ny, int nx) { return get_proxied(X, ny, nx); }
double& rx_py_vector3_field::y(int ny, int nx) { return get_proxied(Y, ny, nx); }
double& rx_py_vector3_field::z(int ny, int nx) { return get_proxied(Z, ny, nx); }

double rx_py_vector3_field::x(int ny, int nx) const { return get_proxied(X, ny, nx); }
double rx_py_vector3_field::y(int ny, int nx) const { return get_proxied(Y, ny, nx); }
double rx_py_vector3_field::z(int ny, int nx) const { return get_proxied(Z, ny, nx); }


cx_py_vector3_field::cx_py_vector3_field(int size_x, int size_y)
  : px_py_vector3_field(size_x, size_y) {}

double& cx_py_vector3_field::x(int ny, int nx) {
  return fx_[index(periodic(ny, 0, size_y_), continuous(nx, 0, size_x_))];
}
double& cx_py_vector3_field::y(int ny, int nx) {
  return fy_[index(periodic(ny, 0, size_y_), continuous(nx, 0, size_x_))];
}
double& cx_py_vector3_field::z(int ny, int nx) {
  return fz_[index(periodic(ny, 0, size_y_), continuous(nx, 0, size_x_))];
}

double cx_py_vector3_field::x(int ny, int nx) const {
  return fx_[index(periodic(ny, 0, size_y_), continuous(nx, 0, size_x_))];
}
double cx_py_vector3_field::y(int ny, int nx) const {
  return fy_[index(periodic(ny, 0, size_y_), continuous(nx, 0, size_x_))];
}
double cx_py_vector3_field::z(int ny, int nx) const {
  return fz_[index(periodic(ny, 0, size_y_), continuous(nx, 0, size_x_))];
}


cx_cy_vector3_field::cx_cy_vector3_field(int size_x, int size_y)
  : px_py_vector3_field(size_x, size_y) {}

double& cx_cy_vector3_field::x(int ny, int nx) {
  return fx_[index(continuous(ny, 0, size_y_), continuous(nx, 0, size_x_))];
}
double& cx_cy_vector3_field::y(int ny, int nx) {
  return fy_[index(continuous(ny, 0, size_y_), continuous(nx, 0, size_x_))];
}
double& cx_cy_vector3_field::z(int ny, int nx) {
  return fz_[index(continuous(ny, 0, size_y_), continuous(nx, 0, size_x_))];
}

double cx_cy_vector3_field::x(int ny, int nx) const {
  return fx_[index(continuous(ny, 0, size_y_), continuous(nx, 0, size_x_))];
}
double cx_cy_vector3_field::y(int ny, int nx) const {
  return fy_[index(continuous(ny, 0, size_y_), continuous(nx, 0, size_x_))];
}
double cx_cy_vector3_field::z(int ny, int nx) const {
  return fz_[index(continuous(ny, 0, size_y_), continuous(nx, 0, size_x_))];
}
