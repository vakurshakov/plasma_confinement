#include "concrete_point_interpolation.hpp"

namespace regular {

Concrete_point_interpolation::Concrete_point_interpolation(
    Uniform_fields_storage&& storage)
    : storage_(std::move(storage)) {}

void Concrete_point_interpolation::process(
    const vector2& r0, vector3& local_E, vector3& local_B) const {
  local_E += storage_.local_E0;
  local_B += storage_.local_B0;
}

}  // namespace regular
