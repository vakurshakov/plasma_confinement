#include "src/fields/fields.hpp"
#include "src/solvers/FDTD.hpp"

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