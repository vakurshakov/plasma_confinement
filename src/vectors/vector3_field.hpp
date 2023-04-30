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
 * @todo Refactor this class to adopting strategies.
 */
class vector3_field {
 public:
  vector3_field() = default;

  vector3_field(int size_x, int size_y);

  virtual ~vector3_field() = default;

  int size_x() const { return size_x_; }
  int size_y() const { return size_y_; }

  virtual int ix_first(Axis axis) const = 0;
  virtual int iy_first(Axis axis) const = 0;
  virtual int ix_last(Axis axis) const = 0;
  virtual int iy_last(Axis axis) const = 0;

  virtual double& x(int ny, int nx) = 0;
  virtual double& y(int ny, int nx) = 0;
  virtual double& z(int ny, int nx) = 0;

  virtual const double& x(int ny, int nx) const = 0;
  virtual const double& y(int ny, int nx) const = 0;
  virtual const double& z(int ny, int nx) const = 0;

  vector3 operator()(int ny, int nx) const;

  double& operator()(Axis axis, int ny, int nx);
  const double& operator()(Axis axis, int ny, int nx) const;

 protected:
  constexpr int index(int ny, int nx) const;

  int size_x_, size_y_;

  std::vector<double> fx_;
  std::vector<double> fy_;
  std::vector<double> fz_;
};


/// @brief Periodic field along both axis
class px_py_vector3_field : public vector3_field {
 public:
  px_py_vector3_field(int size_x, int size_y);

  int ix_first(Axis axis) const override;
  int iy_first(Axis axis) const override;
  int ix_last(Axis axis) const override;
  int iy_last(Axis axis) const override;

  double& x(int ny, int nx) override;
  double& y(int ny, int nx) override;
  double& z(int ny, int nx) override;

  const double& x(int ny, int nx) const override;
  const double& y(int ny, int nx) const override;
  const double& z(int ny, int nx) const override;
};


/// @brief PEC boundary along x-axis and periodic along y-axis
class rx_py_vector3_field : public vector3_field {
 public:
  rx_py_vector3_field(std::string type, int size_x, int size_y);

  int ix_first(Axis axis) const override;
  int iy_first(Axis axis) const override;
  int ix_last(Axis axis) const override;
  int iy_last(Axis axis) const override;

  double& x(int ny, int nx) override;
  double& y(int ny, int nx) override;
  double& z(int ny, int nx) override;

  const double& x(int ny, int nx) const override;
  const double& y(int ny, int nx) const override;
  const double& z(int ny, int nx) const override;

 private:
  struct index_boundaries {
    int left[3];
    int right[3];
  };

  index_boundaries bound_;
  mutable double zero_ = 0.0;

  inline double& get_proxied(Axis axis, int ny, int nx);
  inline const double& get_proxied(Axis axis, int ny, int nx) const;
};


/// @note uses same index boundaries as px_py_vector3_field
class cx_py_vector3_field : public px_py_vector3_field {
 public:
  cx_py_vector3_field(int size_x, int size_y);

  double& x(int ny, int nx) override;
  double& y(int ny, int nx) override;
  double& z(int ny, int nx) override;

  const double& x(int ny, int nx) const override;
  const double& y(int ny, int nx) const override;
  const double& z(int ny, int nx) const override;
};

class cx_cy_vector3_field : public px_py_vector3_field {
 public:
  cx_cy_vector3_field(int size_x, int size_y);

  double& x(int ny, int nx) override;
  double& y(int ny, int nx) override;
  double& z(int ny, int nx) override;

  const double& x(int ny, int nx) const override;
  const double& y(int ny, int nx) const override;
  const double& z(int ny, int nx) const override;
};

#endif  // SRC_VECTORS_VECTOR3_FIELD_HPP
