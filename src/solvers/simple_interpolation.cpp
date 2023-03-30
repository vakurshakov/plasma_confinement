#include "simple_interpolation.hpp"

Simple_interpolation::Simple_interpolation(
    const Parameters& parameters, const v3f& E, const v3f& B)
    : E_(E), B_(B), shape_at_(parameters.form_factor()), charge_cloud_(parameters.charge_cloud()) {}


enum shift {
  no = 0,  // shape[i - x]
  sh = 1   // shape[(i + 0.5) - x]
};

void Simple_interpolation::process(const vector2& r0, vector3& local_E, vector3& local_B) const {
  const int nearest_x = int(round(r0.x()/dx));
  const int nearest_y = int(round(r0.y()/dy));

  vector2 shape[2];

  for(int ny = nearest_y - charge_cloud_; ny <= nearest_y + charge_cloud_; ++ny) {
    shape[shift::no].y() = shape_at_(ny * dy - r0.y(), dy);
    shape[shift::sh].y() = shape_at_((ny + 0.5) * dy - r0.y(), dy);

    for(int nx = nearest_x - charge_cloud_; nx <= nearest_x + charge_cloud_; ++nx) {
      shape[shift::no].x() = shape_at_(nx * dx - r0.x(), dx);
      shape[shift::sh].x() = shape_at_((nx + 0.5) * dx - r0.x(), dx);

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
