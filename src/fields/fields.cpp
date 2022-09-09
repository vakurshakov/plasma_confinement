#include "src/fields/fields.hpp"
#include "src/solvers/FDTD.hpp"

Fields::Fields(Fields_builder& builder) {
  E_ = builder.build_field("Electric");
  B_ = builder.build_field("Magnetic");
  J_ = builder.build_field("Electric");

  boundary_processor_ = builder.build_boundary_processor(*E_, *B_);

  diagnostics_ = builder.build_diagnostics();
}

void Fields::propagate() {
  FDTD_2D(*E_, *B_, *J_);

  boundary_processor_->process();
}

void Fields::diagnose(int t) const {
  #pragma omp parallel for shared(diagnostics_), num_threads(THREAD_NUM)
  for (auto& diagnostic : diagnostics_) {
    diagnostic->diagnose(*E_, *B_, *J_, t);
  }
}