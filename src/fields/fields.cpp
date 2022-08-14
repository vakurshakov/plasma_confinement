#include "fields.hpp"
#include "src/solvers/FDTD.hpp"

namespace regular {

Electromagnetic_field::Electromagnetic_field(int size_x, int size_y)
  : E(size_x, size_y), B(size_x, size_y), J(size_x, size_y) {
}

void Electromagnetic_field::propagate() {
  FDTD_2D(E, B, J);
}

void Electromagnetic_field::set_uniform_E(vector3 uni_E) {
  for (const auto& g : E.indexes(0)) {
    E(0, g) = uni_E.x();
  }

  for (const auto& g : E.indexes(1)) {
    E(1, g) = uni_E.y();
  }

  for (const auto& g : E.indexes(2)) {
    E(2, g) = uni_E.z();
  }
}

void Electromagnetic_field::set_uniform_B(vector3 uni_B) {
  for (const auto& g : B.indexes(0)) {
    B(0, g) = uni_B.x();
  }

  for (const auto& g : B.indexes(1)) {
    B(1, g) = uni_B.y();
  }

  for (const auto& g : B.indexes(2)) {
    B(2, g) = uni_B.z();
  }
}

void Electromagnetic_field::diagnose(int t) const {
  #pragma omp parallel for shared(diagnostics_), num_threads(THREAD_NUM)
  for (auto& diagnostic : diagnostics_) {
    diagnostic->diagnose(t);
  }
}

}  // namespace regular
