#include "src/fields/fields.hpp"

// in 2D-FDTD fields are independent of z, so every % / dz fractions will be zero
void FDTD_2D(vector3_field& E, vector3_field& B, vector3_field& J) {
  PROFILE_FUNCTION();

  #pragma omp parallel shared(E, B, J)
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

    // To ensure all magnetic field components are updated
    #pragma omp barrier

    // Ex(y+1/2, x) at t+1
    #pragma omp for simd nowait
    for (int ny = E.y_min(X); ny < E.y_max(X); ++ny) {
    for (int nx = E.x_min(X); nx < E.x_max(X); ++nx) {
      E.x(ny, nx) += - J.x(ny, nx) * dt +
        (B.z(ny, nx) - B.z(ny-1, nx)) * dt / dy;
    }}

    // Ey(y, x+1/2) at t+1
    #pragma omp for simd nowait
    for (int ny = E.y_min(Y); ny < E.y_max(Y); ++ny) {
    for (int nx = E.x_min(Y); nx < E.x_max(Y); ++nx) {
      E.y(ny, nx) += - J.y(ny, nx) * dt -
        (B.z(ny, nx) - B.z(ny, nx-1)) * dt / dx;
    }}

  #if _2D3V
    // Ez(y+1/2,x+1/2) at t+1
    #pragma omp for simd nowait
    for (int ny = E.y_min(Z); ny < E.y_max(Z); ++ny) {
    for (int nx = E.x_min(Z); nx < E.x_max(Z); ++nx) {
      E.z(ny, nx) += - J.z(ny, nx) * dt + (
        (B.y(ny, nx+1) - B.y(ny, nx)) / dx -
        (B.x(ny+1, nx) - B.x(ny, nx)) / dy) * dt;
    }}
  #endif
  }
}
