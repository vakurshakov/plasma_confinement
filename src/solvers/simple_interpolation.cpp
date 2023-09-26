#include "simple_interpolation.hpp"

Simple_interpolation::Simple_interpolation(const v3f& E, const v3f& B)
    : E_(E), B_(B) {}


enum shift {
  no = 0,  // shape[i - x]
  sh = 1   // shape[(i + 0.5) - x]
};

void Simple_interpolation::process(const vector2& r0, vector3& local_E, vector3& local_B) const {
  const int n_px = int(round(r0.x() / dx));
  const int n_py = int(round(r0.y() / dy));

  vector2 shape[2];

  for(int y = n_py - shape_radius; y <= n_py + shape_radius; ++y) {
    shape[shift::no].y() = shape_function(y * dy - r0.y(), dy);
    shape[shift::sh].y() = shape_function((y + 0.5) * dy - r0.y(), dy);

    for(int x = n_px - shape_radius; x <= n_px + shape_radius; ++x) {
      shape[shift::no].x() = shape_function(x * dx - r0.x(), dx);
      shape[shift::sh].x() = shape_function((x + 0.5) * dx - r0.x(), dx);

      local_E.x() += E_.x(y,x) * shape[shift::no].y() * shape[shift::sh].x();
      local_E.y() += E_.y(y,x) * shape[shift::sh].y() * shape[shift::no].x();
      local_B.z() += B_.z(y,x) * shape[shift::sh].y() * shape[shift::sh].x();

#if _2D3V
      local_E.z() += E_.z(y,x) * shape[shift::sh].y() * shape[shift::sh].x();
      local_B.x() += B_.x(y,x) * shape[shift::no].y() * shape[shift::sh].x();
      local_B.y() += B_.y(y,x) * shape[shift::sh].y() * shape[shift::no].x();
#endif
    }
  }
}
