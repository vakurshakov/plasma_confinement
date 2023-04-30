#include "set_fields_distribution.hpp"

Set_fields_distribution::Set_fields_distribution(
    Fields* fields,
    const vector3& electric_field,
    const vector3& magnetic_field)
    : fields_(fields),
      electric_field_(electric_field),
      magnetic_field_(magnetic_field) {}

void Set_fields_distribution::execute(int /* timestep */) {
  PROFILE_FUNCTION();

  vector3_field& E = fields_->E();
  vector3_field& B = fields_->B();

  for (const auto& axis : Axis()) {
    for (int y = E.iy_first(axis); y < E.iy_last(axis); ++y) {
    for (int x = E.ix_first(axis); x < E.ix_last(axis); ++x) {
      E(axis, y, x) = electric_field_.vec[axis];
    }}

    for (int y = B.iy_first(axis); y < B.iy_last(axis); ++y) {
    for (int x = B.ix_first(axis); x < B.ix_last(axis); ++x) {
      B(axis, y, x) = magnetic_field_.vec[axis];
    }}
  }
}
