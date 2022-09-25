#include "vector3_field.hpp"

inline int periodic(int g, int begin, int end) {
  int pg = g % (end - begin);

  if (g < begin)
    pg += (end - 1);

  return pg;
}

inline bool in_bounds(int x, int begin, int end) {
  return begin <= x && x < end;
}


vector3_field::vector3_field(int size_x, int size_y)
    : size_x_(size_x), size_y_(size_y) {
  field_.reserve(size_x_ * size_y_);
}

double& vector3_field::x(int ny, int nx) { return operator()(X, ny, nx).x(); }
double& vector3_field::y(int ny, int nx) { return operator()(Y, ny, nx).y(); }
double& vector3_field::z(int ny, int nx) { return operator()(Z, ny, nx).z(); }

double vector3_field::x(int ny, int nx) const { return operator()(ny, nx).x(); }
double vector3_field::y(int ny, int nx) const { return operator()(ny, nx).y(); }
double vector3_field::z(int ny, int nx) const { return operator()(ny, nx).z(); }


px_py_vector3_field::px_py_vector3_field(int size_x, int size_y)
    : vector3_field(size_x, size_y) {}

int px_py_vector3_field::ix_first (Axis ax) const { return 0; }
int px_py_vector3_field::iy_first (Axis ax) const { return 0; }
int px_py_vector3_field::ix_last (Axis ax) const { return size_x_; }
int px_py_vector3_field::iy_last (Axis ax) const { return size_y_; }

const vector3& px_py_vector3_field::operator()(int ny, int nx) const {
  int px = periodic(nx, 0, size_x_);
  int py = periodic(ny, 0, size_y_);

  return field_[py * size_x_ + px];
}

vector3& px_py_vector3_field::operator()(Axis axis, int ny, int nx) {
  int px = periodic(nx, ix_first(axis), ix_last(axis));
  int py = periodic(ny, iy_first(axis), iy_last(axis));

  return field_[py * size_x_ + px];
}


rx_ry_vector3_field::rx_ry_vector3_field(std::string type, int size_x, int size_y)
    :  vector3_field(size_x, size_y) {
  if ( type == "Electric" ) {
    bound_.left [X] = 0;       bound_.left [Y] = 1;       bound_.left [Z] = 0;
    bound_.lower[X] = 1;       bound_.lower[Y] = 0;       bound_.lower[Z] = 0;
    bound_.right[X] = size_x_; bound_.right[Y] = size_x;  bound_.right[Z] = size_x_-1;
    bound_.upper[X] = size_y_; bound_.upper[Y] = size_y_; bound_.upper[Z] = size_y_-1;
  }
  else if (type == "Magnetic") {
    bound_.left [X] = 0;         bound_.left [Y] = 0;         bound_.left [Z] = 0;
    bound_.lower[X] = 0;         bound_.lower[Y] = 0;         bound_.lower[Z] = 0;
    bound_.right[X] = size_x_-1; bound_.right[Y] = size_x;    bound_.right[Z] = size_x_;
    bound_.upper[X] = size_y_;   bound_.upper[Y] = size_y_-1; bound_.upper[Z] = size_y_;
  }
}

int rx_ry_vector3_field::ix_first (Axis axis) const { return bound_.left [axis]; }
int rx_ry_vector3_field::iy_first (Axis axis) const { return bound_.lower[axis]; }
int rx_ry_vector3_field::ix_last (Axis axis) const  { return bound_.right[axis]; }
int rx_ry_vector3_field::iy_last (Axis axis) const  { return bound_.upper[axis]; }

const vector3& rx_ry_vector3_field::operator()(int ny, int nx) const {
  if (in_bounds(nx, 0, size_x_) &&
      in_bounds(ny, 0, size_y_)) {
    return field_[ny * size_x_ + nx];
  }
  else {
    // const_cast<vector3&>(zero_) = {0, 0, 0};  // ???
    return zero_;
  };
}

vector3& rx_ry_vector3_field::operator()(Axis axis, int ny, int nx) {
  if (in_bounds(nx, ix_first(axis), ix_last(axis)) &&
      in_bounds(ny, iy_first(axis), iy_last(axis))) {
    return field_[ny * size_x_ + nx];
  }
  else {
    zero_ = {0, 0, 0};
    return zero_;
  }
}