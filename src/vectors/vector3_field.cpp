#include "vector3_field.hpp"

// It doesn't include cases where begin > 0.
inline int periodic(int g, int begin, int end) {
  int pg = g % end;

  if (g < begin)
    pg += end;  // actually, not funny at all

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
    : size_x_(size_x), size_y_(size_y) {
  fx_.reserve(size_x_ * size_y_);
  fy_.reserve(size_x_ * size_y_);
  fz_.reserve(size_x_ * size_y_);
}

constexpr int vector3_field::index(int ny, int nx) const {
  return ny * size_x_ + nx;
}


px_py_vector3_field::px_py_vector3_field(int size_x, int size_y)
  : vector3_field(size_x, size_y) {}

int px_py_vector3_field::ix_first (Axis ax) const { return 0; }
int px_py_vector3_field::iy_first (Axis ax) const { return 0; }
int px_py_vector3_field::ix_last (Axis ax) const { return size_x_; }
int px_py_vector3_field::iy_last (Axis ax) const { return size_y_; }

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

vector3 px_py_vector3_field::operator()(int ny, int nx) const {
  int ind = index(periodic(ny, 0, size_y_), periodic(nx, 0, size_x_));
  return {fx_[ind], fy_[ind], fz_[ind]};
}


rx_ry_vector3_field::rx_ry_vector3_field(std::string type, int size_x, int size_y)
    : vector3_field(size_x, size_y) {
  if ( type == "Electric" ) {
    bound_.left [X] = 0;       bound_.left [Y] = 1;       bound_.left [Z] = 0;
    bound_.lower[X] = 1;       bound_.lower[Y] = 0;       bound_.lower[Z] = 0;
    bound_.right[X] = size_x_; bound_.right[Y] = size_x_; bound_.right[Z] = size_x_-1;
    bound_.upper[X] = size_y_; bound_.upper[Y] = size_y_; bound_.upper[Z] = size_y_-1;
  }
  else if (type == "Magnetic") {
    bound_.left [X] = 0;         bound_.left [Y] = 0;         bound_.left [Z] = 0;
    bound_.lower[X] = 0;         bound_.lower[Y] = 0;         bound_.lower[Z] = 0;
    bound_.right[X] = size_x_-1; bound_.right[Y] = size_x_;   bound_.right[Z] = size_x_;
    bound_.upper[X] = size_y_;   bound_.upper[Y] = size_y_-1; bound_.upper[Z] = size_y_;
  }
}

int rx_ry_vector3_field::ix_first (Axis axis) const { return bound_.left [axis]; }
int rx_ry_vector3_field::iy_first (Axis axis) const { return bound_.lower[axis]; }
int rx_ry_vector3_field::ix_last (Axis axis) const  { return bound_.right[axis]; }
int rx_ry_vector3_field::iy_last (Axis axis) const  { return bound_.upper[axis]; }

/// @todo returning zero is not configured 
double& rx_ry_vector3_field::x(int ny, int nx) { return fx_[index(ny, nx)]; }
double& rx_ry_vector3_field::y(int ny, int nx) { return fy_[index(ny, nx)]; }
double& rx_ry_vector3_field::z(int ny, int nx) { return fz_[index(ny, nx)]; }

double rx_ry_vector3_field::x(int ny, int nx) const { return fx_[index(ny, nx)]; }
double rx_ry_vector3_field::y(int ny, int nx) const { return fy_[index(ny, nx)]; }
double rx_ry_vector3_field::z(int ny, int nx) const { return fz_[index(ny, nx)]; }

vector3 rx_ry_vector3_field::operator()(int ny, int nx) const {
  if (in_bounds(nx, 0, size_x_) && in_bounds(ny, 0, size_y_)) {
    int ind = index(ny, nx);
    return {fx_[ind], fy_[ind], fz_[ind]};
  }
  else return zero_;
}


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

vector3 cx_py_vector3_field::operator()(int ny, int nx) const {
  int ind = index(periodic(ny, 0, size_y_), continuous(nx, 0, size_x_));
  return {fx_[ind], fy_[ind], fz_[ind]};
}