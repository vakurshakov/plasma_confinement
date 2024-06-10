#include "Esirkepov_density_decomposition.hpp"

Esirkepov_density_decomposition::Esirkepov_density_decomposition(
  const Parameters& parameters, vector3_field& J)
  : Np_(parameters.Np()), J_(J) {}


static constexpr int shape_width = 2 * shape_radius + 1;

void Esirkepov_density_decomposition::process(const Particle& particle, const vector2& r0) {
  double n = particle.n();
  double q = particle.q();

  const int node_px = int(round(particle.point.x() / dx)) - shape_radius;
  const int node_py = int(round(particle.point.y() / dy)) - shape_radius;

  decompose_x(particle, r0, n, q, node_px, node_py);
  decompose_y(particle, r0, n, q, node_px, node_py);

#if _2D3V
  decompose_z(particle, r0, n, q, node_px, node_py);
#endif
}

void Esirkepov_density_decomposition::decompose_x(
    const Particle& particle, const vector2& r0,
    double n, double q, int node_px, int node_py) {
  double temp_Jx[shape_width];

  const vector2& r = particle.point.r;

  int node_gx, node_gy;
  node_gx = node_px;

  for (int y = 0; y < shape_width; ++y) {
    node_gy = node_py + y;

    // Give up trying to put (node_gx + 0.5) here
    temp_Jx[y] = - q * n/Np_ * 0.5 * dx / dt *
      (shape_function(r.x() - node_gx * dx, dx) - shape_function(r0.x() - node_gx * dx, dx)) *
      (shape_function(r.y() - node_gy * dy, dy) + shape_function(r0.y() - node_gy * dy, dy));

    #pragma omp atomic
    J_.x(node_gy, node_gx) += temp_Jx[y];
  }

  for (int y = 0; y < shape_width; ++y) {
    node_gy = node_py + y;

    for (int x = 1; x < shape_width; ++x) {
      node_gx = node_px + x;

      // Give up trying to put (node_gx + 0.5) here
      temp_Jx[y] += - q * n/Np_ * 0.5 * dx / dt *
        (shape_function(r.x() - node_gx * dx, dx) - shape_function(r0.x() - node_gx * dx, dx)) *
        (shape_function(r.y() - node_gy * dy, dy) + shape_function(r0.y() - node_gy * dy, dy));

      #pragma omp atomic
      J_.x(node_gy, node_gx) += temp_Jx[y];
    }
  }
}

void Esirkepov_density_decomposition::decompose_y(
    const Particle& particle, const vector2& r0,
    double n, double q, int node_px, int node_py) {
  double temp_Jy[shape_width];

  const vector2& r = particle.point.r;

  int node_gx, node_gy;
  node_gy = node_py;

  for (int x = 0; x < shape_width; ++x) {
    node_gx = node_px + x;

    // Give up trying to put (node_gy + 0.5) here
    temp_Jy[x] = - q * n/Np_ * 0.5 * dy / dt *
      (shape_function(r.x() - node_gx * dx, dx) + shape_function(r0.x() - node_gx * dx, dx)) *
      (shape_function(r.y() - node_gy * dy, dy) - shape_function(r0.y() - node_gy * dy, dy));

    #pragma omp atomic
    J_.y(node_gy, node_gx) += temp_Jy[x];
  }

  for (int y = 1; y < shape_width; ++y) {
    node_gy = node_py + y;

    for (int x = 0; x < shape_width; ++x) {
      node_gx = node_px + x;

      // Give up trying to put (node_gy + 0.5) here
      temp_Jy[x] += - q * n/Np_ * 0.5 * dy / dt *
        (shape_function(r.x() - node_gx * dx, dx) + shape_function(r0.x() - node_gx * dx, dx)) *
        (shape_function(r.y() - node_gy * dy, dy) - shape_function(r0.y() - node_gy * dy, dy));

      #pragma omp atomic
      J_.y(node_gy, node_gx) += temp_Jy[x];
    }
  }
}


void Esirkepov_density_decomposition::decompose_z(
    const Particle& particle, const vector2& r0,
    double n, double q, int node_px, int node_py) {
  double temp_Jz;

  const vector2& r = particle.point.r;
  const vector3& p = particle.point.p;

  double m = particle.m();
  double vz = p.z() / sqrt(m * m + p.dot(p));

  int node_gx, node_gy;

  for (int y = 0; y < shape_width; ++y) {
    node_gy = node_py + y;

    for (int x = 0; x < shape_width; ++x) {
      node_gx = node_px + x;

      temp_Jz = q * n/Np_ * vz * (
        shape_function( r.x() - (node_gx + 0.5) * dx, dx) * shape_function( r.y() - (node_gy + 0.5) * dy, dy) / 3. +
        shape_function(r0.x() - (node_gx + 0.5) * dx, dx) * shape_function( r.y() - (node_gy + 0.5) * dy, dy) / 6. +
        shape_function( r.x() - (node_gx + 0.5) * dx, dx) * shape_function(r0.y() - (node_gy + 0.5) * dy, dy) / 6. +
        shape_function(r0.x() - (node_gx + 0.5) * dx, dx) * shape_function(r0.y() - (node_gy + 0.5) * dy, dy) / 3.);

      #pragma omp atomic
      J_.z(node_gy, node_gx) += temp_Jz;
    }
  }
}
