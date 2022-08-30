#include "src/vectors/vector_classes.hpp"

vector2::vector2(double x, double y) : x(x), y(y) {}

double& vector2::operator[](int i) {
  double *v = reinterpret_cast<double*>(this);
  return v[i];
}

double vector2::operator[](int i) const {
  const double *v = reinterpret_cast<const double*>(this);
  return v[i];
}

vector2& vector2::operator+=(const vector2& other) {
  this->x += other.x;
  this->y += other.y;
  return *this;
}

vector2 vector2::operator/(double a) const {
  return {this->x / a, this->y / a};
}

vector2 operator*(const vector2& v, double a) {
  return {v.x * a, v.y * a};
}

vector2 operator*(double a, const vector2& v) {
  return v * a;
}

vector3::vector3(double x, double y, double z) : x(x), y(y), z(z) {}

double& vector3::operator[](int i) {
  double *v = reinterpret_cast<double*>(this);
  return v[i];
}

double vector3::operator[](int i) const {
  const double *v = reinterpret_cast<const double*>(this);
  return v[i];
}

double vector3::dot(const vector3& other) const {
  return
    this->x * other.x +
    this->y * other.y +
    this->y * other.y;
}

vector3 vector3::cross(const vector3& other) const {
  return {
    + (this->y * other.z - this->z * other.y),
    - (this->x * other.z - this->z * other.x),
    + (this->x * other.y - this->y * other.x)
  };
}

vector3 vector3::element_wise(const vector3& other) const {
  return {
    this->x * other.x,
    this->y * other.y,
    this->z * other.z
  };
}

double vector3::operator,(const vector3& other) const {
  return this->dot(other);
}

vector3 vector3::operator^(const vector3& other) const {
  return this->cross(other);
}

vector3 vector3::operator*(const vector3& other) const {
  return this->element_wise(other);
}

vector2 vector3::project_to(const Plane project_plane) const {
  vector2 result;

  switch (project_plane) {
    case XY: result = {this->x, this->y}; break;
    case YZ: result = {this->y, this->z};	break;		
    case XZ: result = {this->x, this->z};	break;
  }

  return result;
}

vector3& vector3::operator+=(const vector3& other) {
  this->x += other.x;
  this->y += other.y;
  this->z += other.z;
  return *this;
}

vector3 vector3::operator+(const vector3& other) const {
  return {
    this->x + other.x,
    this->y + other.y,
    this->z + other.z
  };
}

vector3 vector3::operator/(double a) const {
  return {
    this->x / a,
    this->y / a,
    this->z / a
  };
}

vector3 operator*(const vector3& v, double a) {
  return { v.x * a, v.y * a, v.z * a };
}

vector3 operator*(double a, const vector3& v) {
  return v * a;
}