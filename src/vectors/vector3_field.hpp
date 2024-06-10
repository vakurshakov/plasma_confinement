#ifndef SRC_VECTORS_VECTOR3_FIELD_HPP
#define SRC_VECTORS_VECTOR3_FIELD_HPP

#include "src/pch.h"
#include "vector_classes.hpp"

/**
 * @brief This class provides an interface for a vector field.
 * It proxies indexing at the border, so simple boundary
 * conditions automatically works.
 *
 * @todo Replace .g(y, x) onto .g(x, y) everywhere.
 */
class vector3_field {
 public:
  vector3_field(int size_x, int size_y);
  vector3_field(const std::string& type, int size_x, int size_y);

  #define OMP_SIMD _Pragma("omp declare simd notinbranch")
  OMP_SIMD constexpr int size_x() const { return size_x_; }
  OMP_SIMD constexpr int size_y() const { return size_y_; }
  #undef OMP_SIMD

  #define OMP_SIMD _Pragma("omp declare simd uniform(axis), notinbranch")
  OMP_SIMD constexpr int x_min(Axis axis) const { return bound_.x_min[axis]; }
  OMP_SIMD constexpr int x_max(Axis axis) const { return bound_.x_max[axis]; }
  OMP_SIMD constexpr int y_min(Axis axis) const { return bound_.y_min[axis]; }
  OMP_SIMD constexpr int y_max(Axis axis) const { return bound_.y_max[axis]; }
  #undef OMP_SIMD


  #define OMP_SIMD                                                       \
    _Pragma("omp declare simd linear(ny, nx: 1), notinbranch")           \
    _Pragma("omp declare simd linear(nx: 1), uniform(ny), notinbranch")  \
    _Pragma("omp declare simd linear(ny: 1), uniform(nx), notinbranch")  \

  OMP_SIMD inline double& x(int ny, int nx);
  OMP_SIMD inline double& y(int ny, int nx);
  OMP_SIMD inline double& z(int ny, int nx);
  OMP_SIMD inline const double& x(int ny, int nx) const;
  OMP_SIMD inline const double& y(int ny, int nx) const;
  OMP_SIMD inline const double& z(int ny, int nx) const;

  OMP_SIMD double& operator()(Axis comp, int ny, int nx);
  OMP_SIMD const double& operator()(Axis comp, int ny, int nx) const;

  vector3 operator()(int ny, int nx) const;

 private:
  OMP_SIMD constexpr int index(int ny, int nx) const;
  #undef OMP_SIMD

  int size_x_;
  int size_y_;

  std::vector<double> fx_;
  std::vector<double> fy_;
  std::vector<double> fz_;

  struct boundaries_indexes {
    int x_min[3];
    int y_min[3];
    int x_max[3];
    int y_max[3];
  };
  boundaries_indexes bound_;
  mutable double zero_ = 0.0;
};

#include "vector3_field.inl"

#endif  // SRC_VECTORS_VECTOR3_FIELD_HPP
