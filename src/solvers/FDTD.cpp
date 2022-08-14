#include "src/solvers/FDTD.hpp"

// Yee lattice assumed. In 2D fields are independent of z, so every
// %/dz fractions is zero. Currents will be reset to zero afterwards.
void FDTD_2D(electric_field& E, magnetic_field& B, electric_field& J) {
#pragma omp parallel shared(E, B, J), num_threads(THREAD_NUM)
{
  /// @warning Magnetic field half step preset must be used with this.
  #pragma omp parallel for
  for (auto& g : B.indexes(0)) {
    B(0, g.x, g.y) -= (E(2, g.x, g.y + 1) - E(2, g.x, g.y)) * 0.5 * dt / dy;
  }

  #pragma omp parallel for
  for (auto& g : B.indexes(1)) {
    B(1, g.x, g.y) += (E(2, g.x + 1, g.y) - E(2, g.x, g.y)) * 0.5 * dt / dx;
  }

  #pragma omp parallel for
  for (auto& g : B.indexes(2)) {
    B(2, g.x, g.y) -= (
      (E(1, g.x + 1, g.y) - E(1, g.x, g.y)) / dx -
      (E(0, g.x, g.y + 1) - E(0, g.x, g.y)) / dy) * 0.5 * dt;
  }

  #pragma omp parallel for
  for (auto& g : E.indexes(0)) {
    E(0, g.x, g.y) += -J(0, g.x, g.y) * dt + (
      B(2, g.x, g.y) - B(2, g.x, g.y - 1)) * dt / dy;

    J(0, g.x, g.y) = 0;
  }

  #pragma omp parallel for
  for (auto& g : E.indexes(1)) {
    E(1, g.x, g.y) += -J(1, g.x, g.y) * dt + (
      B(2, g.x, g.y) - B(2, g.x - 1, g.y)) * dt / dx;

    J(1, g.x, g.y) = 0;
  }

  #pragma omp parallel for
  for (auto& g : E.indexes(2)) {
    E(2, g.x, g.y) += -J(2, g.x, g.y) * dt + (
      (B(1, g.x, g.y) - B(1, g.x - 1, g.y)) / dx -
      (B(0, g.x, g.y) - B(0, g.x, g.y - 1)) / dy) * dt;

    J(2, g.x, g.y) = 0;
  }
}}