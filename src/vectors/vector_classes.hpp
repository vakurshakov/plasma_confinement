#ifndef SRC_VECTORS_VECTOR_CLASSES_HPP
#define SRC_VECTORS_VECTOR_CLASSES_HPP

struct vector2 {
  double x = 0;
  double y = 0;

  vector2() = default;
  vector2(double x, double y);

  double& operator[](int i);
  double operator[](int i) const;

  vector2& operator+=(const vector2& other);
  vector2 operator/(double a) const;
};

vector2 operator*(const vector2& v, double a);
vector2 operator*(double a, const vector2& v);

struct vector3 {
  double x = 0;
  double y = 0;
  double z = 0;

  vector3() = default;
  vector3(double x, double y, double z);

  double& operator[](int i);
  double operator[](int i) const;

  double dot(const vector3& other) const;
  vector3 cross(const vector3& other) const;
  vector3 element_wise(const vector3& other) const;

  double operator,(const vector3& other) const;
  vector3 operator^(const vector3& other) const;
  vector3 operator*(const vector3& other) const;

  vector3& operator+=(const vector3& other);
  vector3 operator+(const vector3& other) const;
  vector3 operator/(double a) const;

  enum Plane { XY = 0, YZ, XZ };
  vector2 project_to(const Plane project_plane) const;
};

vector3 operator*(const vector3& v, double a);
vector3 operator*(double a, const vector3& v);

#endif  // SRC_VECTORS_VECTOR_CLASSES_HPP
