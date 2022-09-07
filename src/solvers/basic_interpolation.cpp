#include "basic_interpolation.hpp"

namespace regular {

enum SHIFT_STATE {
  kNoShift = 0,  // shape((i + 0.0) - x),
  kShifted = 1   // shape((i + 0.5) - x)
};

Basic_interpolation::Basic_interpolation(
    const Particles2D3V& particles,
    const electric_field& E, const magnetic_field& B)
    :	particles(particles),
      fields_E(const_cast<electric_field&>(E)),
      fields_B(const_cast<magnetic_field&>(B)) {}

void Basic_interpolation::process(
    const vector2& r0, vector3& local_E, vector3& local_B) const {
  spline_shape* shape = particles.get_parameters().shape.get();
  vector2 shape_v[2];

  grid_indexes::index nearest{
    int(round(r0.x / dx)),
    int(round(r0.y / dy)),
  };

  for(int x = nearest.x - shape->radius; x <= nearest.x + shape->radius; ++x) {
    shape_v[kNoShift].x = shape->form((x + 0.0) * dx - r0.x, dx);
    shape_v[kShifted].y = shape->form((x + 0.5) * dx - r0.x, dx);

    for(int y = nearest.y - shape->radius; y <= nearest.y + shape->radius; ++y) {
      shape_v[kNoShift].y = shape->form((y + 0.0) * dy - r0.y, dy);
      shape_v[kShifted].y = shape->form((y + 0.5) * dy - r0.y, dy);

      local_E.x += fields_E(0, x, y) * (shape_v[kShifted].x * shape_v[kNoShift].y);
      local_E.y += fields_E(1, x, y) * (shape_v[kNoShift].x * shape_v[kShifted].y);
      local_E.z += fields_E(2, x, y) * (shape_v[kNoShift].x * shape_v[kNoShift].y);

      local_B.x += fields_B(0, x, y) * (shape_v[kNoShift].x * shape_v[kShifted].y);
      local_B.y += fields_B(1, x, y) * (shape_v[kShifted].x * shape_v[kNoShift].y);
      local_B.z += fields_B(2, x, y) * (shape_v[kShifted].x * shape_v[kShifted].y);
    }
  }
}

}  // namespace regular
