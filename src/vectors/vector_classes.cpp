#include "vector_classes.hpp"

vector2::vector2(double x, double y)
  : vec{x, y} {}

vector2& vector2::operator+=(const vector2& other) {
  this->vec[X] += other.vec[X];
  this->vec[Y] += other.vec[Y];
  return *this;
}

vector2 vector2::operator/(double a) const {
  return (*this) * (1.0 / a);
}

vector2 operator*(const vector2& v, double a) {
  return {
    v.vec[X] * a,
    v.vec[Y] * a
  };
}

vector2 operator*(double a, const vector2& v) {
  return v * a;
}


vector3::vector3(double x, double y, double z)
  : vec{x, y, z} {}

double vector3::dot(const vector3 &other) const {
  return
    this->vec[X] * other.vec[X] +
    this->vec[Y] * other.vec[Y] +
    this->vec[Z] * other.vec[Z];
}

vector3 vector3::cross(const vector3 &other) const {
  return {
    + (this->vec[Y] * other.vec[Z] - this->vec[Z] * other.vec[Y]),
    - (this->vec[X] * other.vec[Z] - this->vec[Z] * other.vec[X]),
    + (this->vec[X] * other.vec[Y] - this->vec[Y] * other.vec[X])
  };
}

vector3 vector3::element_wise(const vector3 &other) const {
  return {
    this->vec[X] * other.vec[X],
    this->vec[Y] * other.vec[Y],
    this->vec[Z] * other.vec[Z]
  };
}

vector3& vector3::operator+=(const vector3& other) {
  this->vec[X] += other.vec[X];
  this->vec[Y] += other.vec[Y];
  this->vec[Z] += other.vec[Z];
  return *this;
}

vector3 vector3::operator+(const vector3& other) const {
  return {
    this->vec[X] + other.vec[X],
    this->vec[Y] + other.vec[Y],
    this->vec[Z] + other.vec[Z]
  };
}

vector3 vector3::operator/(double a) const {
  return (*this) * (1.0 / a);
}

vector2 vector3::squeeze(const Axes axes) const {
  vector2 temp;

  switch (axes) {
    case XY: temp = {vec[X], vec[Y]}; break;
    case YZ: temp = {vec[Y], vec[Z]}; break;
    case XZ: temp = {vec[X], vec[Z]}; break;
  }

  return temp;
}

vector3 operator*(const vector3& v, double a) {
  return {
    v.vec[X] * a,
    v.vec[Y] * a,
    v.vec[Z] * a
  };
}

vector3 operator*(double a, const vector3& v) {
  return v * a;
}
