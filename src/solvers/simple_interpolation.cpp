#include "simple_interpolation.hpp"

Simple_interpolation::Simple_interpolation(const v3f& E, const v3f& B)
    : E_(E), B_(B) {}


enum shift {
  no = 0,  // shape[i - x]
  sh = 1   // shape[(i + 0.5) - x]
};

void Simple_interpolation::process(const vector2& r0, vector3& local_E, vector3& local_B) const {
  const int nearest_x = int(round(r0.x()/dx));
  const int nearest_y = int(round(r0.y()/dy));

  vector2 shape[2];

  for(int ny = nearest_y - shape_radius; ny <= nearest_y + shape_radius; ++ny) {
    shape[shift::no].y() = shape_function(ny * dy - r0.y(), dy);
    shape[shift::sh].y() = shape_function((ny + 0.5) * dy - r0.y(), dy);

    for(int nx = nearest_x - shape_radius; nx <= nearest_x + shape_radius; ++nx) {
      shape[shift::no].x() = shape_function(nx * dx - r0.x(), dx);
      shape[shift::sh].x() = shape_function((nx + 0.5) * dx - r0.x(), dx);

      local_E.x() += E_.x(ny,nx) * shape[shift::no].y() * shape[shift::sh].x();
      local_E.y() += E_.y(ny,nx) * shape[shift::sh].y() * shape[shift::no].x();
      local_B.z() += B_.z(ny,nx) * shape[shift::sh].y() * shape[shift::sh].x();

#if _2D3V
      local_E.z() += E_.z(ny,nx) * shape[shift::sh].y() * shape[shift::sh].x();
      local_B.x() += B_.x(ny,nx) * shape[shift::no].y() * shape[shift::sh].x();
      local_B.y() += B_.y(ny,nx) * shape[shift::sh].y() * shape[shift::no].x();
#endif
    }
  }
}
