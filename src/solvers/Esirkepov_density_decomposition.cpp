#include "Esirkepov_density_decomposition.hpp"

Esirkepov_density_decomposition::Esirkepov_density_decomposition(
  const Parameters& parameters, vector3_field& J)
  : Np_(parameters.Np()), J_(J) {}

static const int shape_width = 2 * shape_radius + 1;

void Esirkepov_density_decomposition::process(const Particle& particle, const vector2& r0) {
  double n = particle.n();
  double q = particle.q();

  const int node_px = int(round(particle.point.x() / dx));
  const int node_py = int(round(particle.point.y() / dy));

  v3f temp_J(shape_width, shape_width);

  decompose_x(particle, r0, n, q, node_px, node_py, temp_J);
  decompose_y(particle, r0, n, q, node_px, node_py, temp_J);

#if _2D3V
  decompose_z(particle, r0, n, q, node_px, node_py, temp_J);
#endif
}

void Esirkepov_density_decomposition::decompose_x(
    const Particle& particle, const vector2& r0,
    double n, double q, int node_px, int node_py, v3f& temp_J) {
  const vector2& r = particle.point.r;

  int node_gx, node_gy;
  node_gx = node_px - shape_radius;

  for (int x = 0, y = 0; y < shape_width; ++y) {
    node_gy = node_py + (y - shape_radius);

    // Give up trying to put (node_gx + 0.5) here
    temp_J.x(y,x) = - q * n/Np_ * 0.5 * dx / dt *
      (shape_function(r.x() - node_gx * dx, dx) - shape_function(r0.x() - node_gx * dx, dx)) *
      (shape_function(r.y() - node_gy * dy, dy) + shape_function(r0.y() - node_gy * dy, dy));

    #pragma omp atomic
    J_.x(node_gy,node_gx) += temp_J.x(y,x);
  }

  for (int y = 0; y < shape_width; ++y) {
    node_gy = node_py + (y - shape_radius);

    for (int x = 1; x < shape_width; ++x) {
      node_gx = node_px + (x - shape_radius);

      // Here is only "=" sign (not "+=")
      // Give up trying to put (node_gx + 0.5) here
      temp_J.x(y,x) = temp_J.x(y,x-1) - q * n/Np_ * 0.5 * dx / dt *
        (shape_function(r.x() - node_gx * dx, dx) - shape_function(r0.x() - node_gx * dx, dx)) *
        (shape_function(r.y() - node_gy * dy, dy) + shape_function(r0.y() - node_gy * dy, dy));

      #pragma omp atomic
      J_.x(node_gy, node_gx) += temp_J.x(y,x);
    }
  }
}

void Esirkepov_density_decomposition::decompose_y(
    const Particle& particle, const vector2& r0,
    double n, double q, int node_px, int node_py, v3f& temp_J) {
  const vector2& r = particle.point.r;

  int node_gx, node_gy;
  node_gy = node_py - shape_radius;

  for (int y = 0, x = 0; x < shape_width; ++x) {
    node_gx = node_px + (x - shape_radius);

    // Give up trying to put (node_gy + 0.5) here
    temp_J.y(y,x) = - q * n/Np_ * 0.5 * dy / dt *
      (shape_function(r.x() - node_gx * dx, dx) + shape_function(r0.x() - node_gx * dx, dx)) *
      (shape_function(r.y() - node_gy * dy, dy) - shape_function(r0.y() - node_gy * dy, dy));

    #pragma omp atomic
    J_.y(node_gy, node_gx) += temp_J.y(y,x);
  }

  for (int y = 1; y < shape_width; ++y) {
    node_gy = node_py + (y - shape_radius);

    for (int x = 0; x < shape_width; ++x) {
      node_gx = node_px + (x - shape_radius);

      // Here is only "=" sign (not "+=")
      // Give up trying to put (node_gy + 0.5) here
      temp_J.y(y,x) = temp_J.y(y-1,x) - q * n/Np_ * 0.5 * dy / dt *
        (shape_function(r.x() - node_gx * dx, dx) + shape_function(r0.x() - node_gx * dx, dx)) *
        (shape_function(r.y() - node_gy * dy, dy) - shape_function(r0.y() - node_gy * dy, dy));

      #pragma omp atomic
      J_.y(node_gy, node_gx) += temp_J.y(y,x);
    }
  }
}


void Esirkepov_density_decomposition::decompose_z(
    const Particle& particle, const vector2& r0,
    double n, double q, int node_px, int node_py, v3f& temp_J) {
  double m = particle.m();

  const vector2& r = particle.point.r;
  const vector3& p = particle.point.p;

  double gamma_m = sqrt(m * m + p.dot(p));
  double vz = p.z() / gamma_m;

  int node_gx, node_gy;

  for (int y = 0; y < shape_width; ++y) {
    node_gy = node_py + (y - shape_radius);

    for (int x = 0; x < shape_width; ++x) {
      node_gx = node_px + (x - shape_radius);

      temp_J.z(y,x) = q * n/Np_ * vz * (
        shape_function( r.x() - (node_gx + 0.5) * dx, dx) * shape_function( r.y() - (node_gy + 0.5) * dy, dy) / 3. +
        shape_function(r0.x() - (node_gx + 0.5) * dx, dx) * shape_function( r.y() - (node_gy + 0.5) * dy, dy) / 6. +
        shape_function( r.x() - (node_gx + 0.5) * dx, dx) * shape_function(r0.y() - (node_gy + 0.5) * dy, dy) / 6. +
        shape_function(r0.x() - (node_gx + 0.5) * dx, dx) * shape_function(r0.y() - (node_gy + 0.5) * dy, dy) / 3.);

      #pragma omp atomic
      J_.z(node_gy, node_gx) += temp_J.z(y,x);
    }
  }
}
