#ifndef SRC_VECTORS_EM_FIELDS_HPP
#define SRC_VECTORS_EM_FIELDS_HPP

#include "src/vectors/basic_field.hpp"
#include "src/vectors/grid_indexes.hpp"
#include "src/utils/assertions.hpp"

/**
 * @brief Electric field specification of `basic_field`.
 * 
 * @details Simplifies adjoint indexing to help in the
 *  process of solving Maxwell's equation with FDTD.
 * 
 *  The `begin` and `end` methods provide indexes so
 *  that only the tangential components at the boundary
 *  are present.
 */
class electric_field : public basic_field<3> {
 public:
  electric_field(int size_x, int size_y)
    : basic_field(size_x, size_y) {}

  int begin(int comp, SpaceDim axis) const final {
    int electric_first = 0;
    if ((comp == 2) ||
        (comp == 0 && axis == SpaceDim::kY) ||
        (comp == 1 && axis == SpaceDim::kX)) {
      electric_first = 1;
    }

    return electric_first;
  }

  int end(int comp, SpaceDim axis) const final {
    int electric_last = size_[axis];
    if ((comp == 0 && axis == SpaceDim::kX) ||
        (comp == 1 && axis == SpaceDim::kY)) {
      electric_last = size_[axis] - 1;
    }

    return electric_last;
  }

  grid_indexes indexes(int comp) const final {
    ASSERT(comp < 3, "Field dimension is less then a given component.");

    int electric_bx = this->begin(comp, kX);
    int electric_by = this->begin(comp, kY);
    int electric_ex = this->end(comp, kX);
    int electric_ey = this->end(comp, kY);
    return grid_indexes{{electric_bx, electric_by}, {electric_ex, electric_ey}};
  }
};

/**
 * @brief Magnetic field specification of `basic_field`.
 * 
 * @details The `begin` and `end` methods here provide
 *  indexes so that only perpendicular components at
 *  the boundary are present.
 */
class magnetic_field : public basic_field<3> {
 public:
  magnetic_field(int size_x, int size_y)
    : basic_field(size_x, size_y) {}

  int begin(int comp, SpaceDim axis) const final {
    return 0;
  }

  int end(int comp, SpaceDim axis) const final {
    int magnetic_last = size_[axis];
    if ((comp == 2) ||
        (comp == 0 && axis == SpaceDim::kY) ||
        (comp == 1 && axis == SpaceDim::kX)) {
      magnetic_last = size_[axis] - 1;
    }

    return magnetic_last;
  }

  grid_indexes indexes(int comp) const final {
    ASSERT(comp < 3, "Field dimension is less then a given component.");

    int magnetic_bx = this->begin(comp, kX);
    int magnetic_by = this->begin(comp, kY);
    int magnetic_ex = this->end(comp, kX);
    int magnetic_ey = this->end(comp, kY);
    return grid_indexes{{magnetic_bx, magnetic_by}, {magnetic_ex, magnetic_ey}};
  }
};

#endif  // SRC_VECTORS_EM_FIELDS_HPP
