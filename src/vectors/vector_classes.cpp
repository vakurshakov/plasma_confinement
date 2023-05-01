#include "vector_classes.hpp"

#include <cmath>

Axis operator++(Axis& axis) {
  return axis = static_cast<Axis>(axis + 1);
}

Axis operator*(Axis axis) {
  return axis;
}

Axis begin(Axis _) {
  return Axis::X;
}

Axis end(Axis _) {
  Axis last = Axis::Z;
  return ++last;
}


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

const vector3 vector3::zero = {0.0, 0.0, 0.0};
const vector3 vector3::orth_x = {1.0, 0.0, 0.0};
const vector3 vector3::orth_y = {0.0, 1.0, 0.0};
const vector3 vector3::orth_z = {0.0, 0.0, 1.0};

double vector3::dot(const vector3 &other) const {
  return
    this->vec[X] * other.vec[X] +
    this->vec[Y] * other.vec[Y] +
    this->vec[Z] * other.vec[Z];
}

double vector3::square() const {
  return this->dot(*this);
}

double vector3::l2_norm() const {
  return sqrt(this->square());
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

bool vector3::operator==(const vector3& other) const {
  static const double equal_tolerance = 1e-8;
  return
    fabs(this->vec[X] - other.vec[X]) < equal_tolerance &&
    fabs(this->vec[Y] - other.vec[Y]) < equal_tolerance &&
    fabs(this->vec[Z] - other.vec[Z]) < equal_tolerance;
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
