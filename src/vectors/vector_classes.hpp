#ifndef SRC_VECTORS_VECTOR_CLASSES_HPP
#define SRC_VECTORS_VECTOR_CLASSES_HPP

enum Axis { X = 0, Y, Z };
Axis operator++(Axis& axis);
Axis operator*(Axis axis);
Axis begin(Axis _);
Axis end(Axis _);

enum Axes { XY = 0, YZ, XZ };

struct vector2 {
  vector2() = default;
  vector2(double x, double y);

  inline double& x() { return vec[X]; }
  inline double& y() { return vec[Y]; }

  inline double x() const { return vec[X]; }
  inline double y() const { return vec[Y]; }

  vector2& operator+=(const vector2& other);
  vector2 operator/(double a) const;

  double vec[2] = {0.0, 0.0};
};

vector2 operator*(const vector2& v, double a);
vector2 operator*(double a, const vector2& v);

struct vector3 {
  vector3() = default;
  vector3(double x, double y, double z);

  inline double& x() { return vec[X]; }
  inline double& y() { return vec[Y]; }
  inline double& z() { return vec[Z]; }

  inline double x() const { return vec[X]; }
  inline double y() const { return vec[Y]; }
  inline double z() const { return vec[Z]; }

  double dot(const vector3& other) const;
  double square() const;
  double l2_norm() const;

  vector3 cross(const vector3& other) const;
  vector3 element_wise(const vector3& other) const;

  vector3& operator+=(const vector3& other);
  vector3 operator+(const vector3& other) const;
  vector3 operator/(double a) const;
  bool operator==(const vector3& other) const;

  vector2 squeeze(const Axes axes) const;

  double vec[3] = {0.0, 0.0, 0.0};
  static const vector3 zero;
  static const vector3 orth_x;
  static const vector3 orth_y;
  static const vector3 orth_z;
};

vector3 operator*(const vector3& v, double a);
vector3 operator*(double a, const vector3& v);

#endif  // SRC_VECTORS_VECTOR_CLASSES_HPP
