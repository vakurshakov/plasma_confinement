#include "src/fields/fields.hpp"

Fields::Fields(Fields_builder& builder) {
  E_ = builder.build_field("Electric");
  B_ = builder.build_field("Magnetic");
  J_ = builder.build_field("Electric");

  boundary_processor_ = builder.build_boundary_processor(*E_, *B_);
}

void Fields::propagate() {
  PROFILE_FUNCTION();

  FDTD_2D(*E_, *B_, *J_);

  boundary_processor_->process();
}

void Fields::clear_sources() {
  PROFILE_FUNCTION();

  auto& J = *J_;

  #pragma omp parallel for
  for (int ny = 0; ny < SIZE_Y; ++ny) {
  for (int nx = 0; nx < SIZE_X; ++nx) {
    J.x(ny, nx) = 0.0;
    J.y(ny, nx) = 0.0;
    J.z(ny, nx) = 0.0;
  }}
}
