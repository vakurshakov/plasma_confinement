#include "magnetic_field_half_step.hpp"
#include <omp.h>

void Magnetic_field_half_step::execute(int /* timestep */) {
  vector3_field& E = fields_->E();
  vector3_field& B = fields_->B();

#pragma omp parallel shared(E, B), num_threads(OMP_NUM_THREADS)
{
#if _2D3V
  // Bx(y, x+1/2) at t+1/2
  #pragma omp for
  for (int ny = B.iy_first(X); ny < B.iy_last(X); ++ny) {
    for (int nx = B.ix_first(X); nx < B.ix_last(X); ++nx) {
      B.x(ny, nx) -= 0.5 * (E.z(ny, nx) - E.z(ny-1, nx)) * dt / dy;
    }
  }

  // By(y+1/2, x) at t+1/2
  #pragma omp for
  for (int ny = B.iy_first(Y); ny < B.iy_last(Y); ++ny) {
    for (int nx = B.ix_first(Y); nx < B.ix_last(Y); ++nx) {
      B.y(ny, nx) += 0.5 * (E.z(ny, nx) - E.z(ny, nx-1)) * dt / dx;
    }
  }
#endif

  // Bz(y, x) at t+1/2
  #pragma omp for
  for (int ny = B.iy_first(Z); ny < B.iy_last(Z); ++ny) {
    for (int nx = B.ix_first(Z); nx < B.ix_last(Z); ++nx) {
      B.z(ny, nx) -= 0.5 * (
        (E.y(ny, nx+1) - E.y(ny, nx)) / dx -
        (E.x(ny+1, nx) - E.x(ny, nx)) / dy) * dt;
    }
  }
}
}
