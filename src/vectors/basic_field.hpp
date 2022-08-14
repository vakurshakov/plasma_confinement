#ifndef SRC_VECTORS_BASIC_FIELD_HPP
#define SRC_VECTORS_BASIC_FIELD_HPP

#include "src/pch.h"
#include "src/vectors/grid_indexes.hpp"
#include "src/utils/assertions.hpp"

enum SpaceDim : std::int8_t {
  kX     = 0,
  kY     = 1,
  kTotal = 2
};

/**
 * @brief Basic field class wraps index-access and
 *  simplifies work with its boundaries.
 * 
 * @details A two-dimensional grid is assumed, field
 *  dimension can vary, meaning that this is a tensor
 *  field. Row-major indexing used internally.
 * 
 * @tparam tensor_dim Dimension of the field. 
 */
template<std::int8_t tensor_dim>
class basic_field {
 public:
  basic_field(int size_x, int size_y)
    : size_{size_x, size_y}, data_(size_x * size_y * tensor_dim, 0) {}

  // Derivatives can override the begin, end and indexes methods.
  virtual ~basic_field() = default;

  // Returns the size of the array along given `axis`.
  int size(SpaceDim axis) const {
    return size_[axis];
  }

  // Returns the number of components in a tensor.
  int field_dim() const {
    return tensor_dim;
  }

  // Indexing routine, returns the field component `comp`
  // for given grid indexes `x` and `y`. Array boundary misses
  // is checked using defined asserts.
  double& operator()(int comp, int x, int y) {
    ASSERT(comp < tensor_dim, "Field dimension is less then a given component.");
    ASSERT((x >= 0 && x < size_[kX]) && (y >= 0 && y < size_[kY]),
      "Given indexes misses the array bounds.");
    return data_[indexing_impl(comp, tensor_dim, x, size_[kX], y, size_[kY])];
  }

  double operator()(int comp, int x, int y) const {
    ASSERT(comp < tensor_dim, "Field dimension is less then a given component.");
    ASSERT((x >= 0 && x < size_[kX]) && (y >= 0 && y < size_[kY]),
      "Given indexes misses the bounds.");
    return data_[indexing_impl(comp, tensor_dim, x, size_[kX], y, size_[kY])];
  }

  // Alias version of `operator()(int comp, int x, int y)`
  double& operator()(int comp, grid_indexes::index g) {
    return data_[indexing_impl(comp, tensor_dim, g.x, size_[kX], g.y, size_[kY])];
  }

  double operator()(int comp, grid_indexes::index g) const {
    return data_[indexing_impl(comp, tensor_dim, g.x, size_[kX], g.y, size_[kY])];
  }

  // Reinterprets `plain` index as an index set,
  // component part is ignored.
  grid_indexes::index to_grid(int plain) {
    int g_part = plain / tensor_dim;
    int ix = g_part / size_[kY];
    int iy = g_part - ix;
    return {ix, iy};
  }

  // Returns the first index to iterate along the
  // given `axis`, depending on field component `comp`.
  virtual int begin(int comp, SpaceDim axis) const {
    return 0;
  }

  // Returns the next to the last index for the
  // given `axis`, depending on field component `comp`.
  virtual int end(int comp, SpaceDim axis) const {
    return size_[axis];
  }

  /**
   * @brief Gives the correct set of indexes for
   *  the specified field component `comp`.
   * 
   * @example So, you can iterate through like this:  
   *  #pragma omp parallel for
   *  for (auto& g : field.indexes(comp)) {...}
   */
  virtual grid_indexes indexes(int comp) const {
    ASSERT(comp < tensor_dim, "Field dimension is less then a given component.");
    return grid_indexes{{0, 0}, {size_[kX], size_[kY]}};
  }

 protected:
  inline constexpr int indexing_impl(
    int f_comp, int f_dim, int x, int size_x, int y, int size_y) {
    return (x * size_y + y) * f_dim + f_comp;
  }

  int size_[kTotal];
  std::vector<double> data_;
};

#endif  // SRC_VECTORS_BASIC_FIELD_HPP
