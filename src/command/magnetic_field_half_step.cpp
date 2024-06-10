#include "magnetic_field_half_step.hpp"
#include <omp.h>

void Magnetic_field_half_step::execute(int /* timestep */) {
  vector3_field& E = fields_->E();
  vector3_field& B = fields_->B();

  #pragma omp parallel shared(E, B)
  {
  #if _2D3V
    // Bx(y, x+1/2) at t+1/2
    #pragma omp for simd nowait
    for (int ny = B.y_min(X); ny < B.y_max(X); ++ny) {
    for (int nx = B.x_min(X); nx < B.x_max(X); ++nx) {
      B.x(ny, nx) -= 0.5 * (E.z(ny, nx) - E.z(ny-1, nx)) * dt / dy;
    }}

    // By(y+1/2, x) at t+1/2
    #pragma omp for simd nowait
    for (int ny = B.y_min(Y); ny < B.y_max(Y); ++ny) {
    for (int nx = B.x_min(Y); nx < B.x_max(Y); ++nx) {
      B.y(ny, nx) += 0.5 * (E.z(ny, nx) - E.z(ny, nx-1)) * dt / dx;
    }}
  #endif

    // Bz(y, x) at t+1/2
    #pragma omp for simd nowait
    for (int ny = B.y_min(Z); ny < B.y_max(Z); ++ny) {
    for (int nx = B.x_min(Z); nx < B.x_max(Z); ++nx) {
      B.z(ny, nx) -= 0.5 * (
        (E.y(ny, nx+1) - E.y(ny, nx)) / dx -
        (E.x(ny+1, nx) - E.x(ny, nx)) / dy) * dt;
    }}
  }
}
