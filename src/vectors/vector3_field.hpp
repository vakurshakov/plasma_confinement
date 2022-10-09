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

  virtual int ix_first (Axis ax) const = 0;
  virtual int iy_first (Axis ax) const = 0;
  virtual int ix_last	(Axis ax) const = 0;
  virtual int iy_last	(Axis ax) const = 0;

  double& x(int ny, int nx);
  double& y(int ny, int nx);
  double& z(int ny, int nx);

  double x(int ny, int nx) const;
  double y(int ny, int nx) const;
  double z(int ny, int nx) const;

  virtual vector3& operator()(Axis ax, int ny, int nx) = 0;
  virtual const vector3& operator()(int ny, int nx) const = 0;

 protected:
  int size_x_, size_y_;
  std::vector<vector3> field_;
};


class px_py_vector3_field : public vector3_field {
 public:
  px_py_vector3_field(int size_x, int size_y);

  int ix_first (Axis ax) const override;
  int iy_first (Axis ax) const override;
  int ix_last	(Axis ax) const override;
  int iy_last	(Axis ax) const override;

  vector3& operator()(Axis ax, int ny, int nx) override;
  const vector3& operator()(int ny, int nx) const override;
};


class rx_ry_vector3_field : public vector3_field {
 public:
  rx_ry_vector3_field(std::string type, int size_x, int size_y);

  int ix_first (Axis ax) const override;
  int iy_first (Axis ax) const override;
  int ix_last	(Axis ax) const override;
  int iy_last	(Axis ax) const override;

  vector3& operator()(Axis ax, int ny, int nx) override;
  const vector3& operator()(int ny, int nx) const override;

 private:
  struct index_boundaries {
    int left[3];
    int lower[3];
    int right[3];
    int upper[3];
  };

  index_boundaries bound_;
  vector3 zero_ = {0, 0, 0};
};

class cx_py_vector3_field : public px_py_vector3_field {
 public:
  cx_py_vector3_field(int size_x, int size_y);

  vector3& operator()(Axis /* axis */, int ny, int nx) override;
  const vector3& operator()(int ny, int nx) const override;
};

#endif  // SRC_VECTORS_VECTOR3_FIELD_HPP
