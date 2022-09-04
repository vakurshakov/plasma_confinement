#ifndef SRC_UTILS_FIELD_PROXY_HPP
#define SRC_UTILS_FIELD_PROXY_HPP

#include "src/pch.h"
#include "src/vectors/basic_field.hpp"
#include "src/vectors/grid_indexes.hpp"

// TODO: Test whether int(*)(int, int, int) is faster.
using boundary_handler = std::function<int(int, int, int)>;

inline int periodic(int x, int begin, int end) {
  int px = x % (end - begin);

  if (x < begin)
    px += (end - 1);

  return px;
}

inline int reflective(int x, int begin, int end) {
  return begin <= x && x < end ? x : -1;
}

template<std::int8_t tensor_dim>
class field_proxy {
 public:
  field_proxy(basic_field<tensor_dim>& field)
    : ref_field(field) {}

  void set_bounds(boundary_handler other_x, boundary_handler other_y) {
    this->bound_x = other_x;
    this->bound_y = other_y;
  }

#define GET_BOUNDED_INDEXES(comp, x, y)                                    \
  int bx = bound_x(x, ref_field.begin(comp, kX), ref_field.end(comp, kX)); \
  int by = bound_y(y, ref_field.begin(comp, kY), ref_field.end(comp, kY))  \

  bool is_writable(int comp, int x, int y) const {
    GET_BOUNDED_INDEXES(comp, x, y);
    return bx >= 0 && by >= 0;
  }

  double& operator()(int comp, int x, int y) {
    GET_BOUNDED_INDEXES(comp, x, y);
    return ref_field(comp, bx, by);
  }

  double& operator()(int comp, const grid_indexes::index& g) {
    GET_BOUNDED_INDEXES(comp, x, y);
    return ref_field(comp, bx, by);
  }

  double operator()(int comp, int x, int y) const {
    GET_BOUNDED_INDEXES(comp, x, y);
    return is_readable(bx, by) ? ref_field(comp, bx, by) : 0;
  }

  double operator()(int comp, const grid_indexes::index& g) const {
    GET_BOUNDED_INDEXES(comp, x, y);
    return is_readable(bx, by) ? ref_field(comp, bx, by) : 0;
  }

 private:
  bool is_readable(int bx, int by) {
    return bx >= 0 && by >= 0;
  }

  basic_field<tensor_dim>& ref_field;
  boundary_handler bound_x = nullptr;
  boundary_handler bound_y = nullptr;
};

#undef GET_BOUNDED_INDEXES(comp, x, y)

#endif  // SRC_UTILS_FIELD_PROXY_HPP
