#include "simple_interpolation.hpp"

Simple_interpolation::Simple_interpolation(const v3f& E, const v3f& B)
    : E_(E), B_(B) {}


static constexpr int shape_width = 2 * shape_radius + 1;

enum Shift {
  NO = 0,  // shape[i - x]
  SH = 1   // shape[(i + 0.5) - x]
};

#pragma omp declare simd linear(y, x: 1), notinbranch
constexpr int index(int y, int x, int comp, int shift) {
  return ((y * shape_width + x) * 2 + comp) * 2 + shift;
}

void Simple_interpolation::process(const vector2& r0, vector3& local_E, vector3& local_B) const {
  double shape[shape_width * shape_width * 4];

  const int node_px = int(round(r0.x() / dx)) - shape_radius;
  const int node_py = int(round(r0.y() / dy)) - shape_radius;

  int node_gx, node_gy;

  #pragma omp simd collapse(2)
  for(int y = 0; y < shape_width; ++y) {
    for(int x = 0; x < shape_width; ++x) {
      node_gx = node_px + x;
      node_gy = node_py + y;

      shape[index(y, x, X, NO)] = shape_function(r0.x() - node_gx * dx, dx);
      shape[index(y, x, Y, NO)] = shape_function(r0.y() - node_gy * dy, dy);
      shape[index(y, x, X, SH)] = shape_function(r0.x() - (node_gx + 0.5) * dx, dx);
      shape[index(y, x, Y, SH)] = shape_function(r0.y() - (node_gy + 0.5) * dy, dy);
    }
  }

  for(int y = 0; y < shape_width; ++y) {
    for(int x = 0; x < shape_width; ++x) {
      node_gx = node_px + x;
      node_gy = node_py + y;

      local_E.x() += E_.x(node_gy, node_gx) * shape[index(y, x, Y, NO)] * shape[index(y, x, X, SH)];
      local_E.y() += E_.y(node_gy, node_gx) * shape[index(y, x, Y, SH)] * shape[index(y, x, X, NO)];
      local_B.z() += B_.z(node_gy, node_gx) * shape[index(y, x, Y, SH)] * shape[index(y, x, X, SH)];

#if _2D3V
      local_E.z() += E_.z(node_gy, node_gx) * shape[index(y, x, Y, SH)] * shape[index(y, x, X, SH)];
      local_B.x() += B_.x(node_gy, node_gx) * shape[index(y, x, Y, NO)] * shape[index(y, x, X, SH)];
      local_B.y() += B_.y(node_gy, node_gx) * shape[index(y, x, Y, SH)] * shape[index(y, x, X, NO)];
#endif
    }
  }
}
