#include "Esirkepov_density_decomposition.hpp"

Esirkepov_density_decomposition::Esirkepov_density_decomposition(
  const Parameters& parameters, vector3_field& J)
  : Np_(parameters.Np()), shape_at_(parameters.form_factor()),
    charge_cloud_(parameters.charge_cloud()), J_(J) {}


void Esirkepov_density_decomposition::process(const Particle& particle, const vector2& r0) {
  double n = particle.n();
  double q = particle.q();

  const int nearest_edge_to_rx = int(round(particle.point.x() / dx));
  const int nearest_edge_to_ry = int(round(particle.point.y() / dy));

  p_v3f temp_J(2 * charge_cloud_ + 1, 2 * charge_cloud_ + 1);

  decompose_x(particle, r0, n, q, nearest_edge_to_rx, nearest_edge_to_ry, temp_J);
  decompose_y(particle, r0, n, q, nearest_edge_to_rx, nearest_edge_to_ry, temp_J);

#if _2D3V
  decompose_z(particle, r0, n, q, nearest_edge_to_rx, nearest_edge_to_ry, temp_J);
#endif
}


void Esirkepov_density_decomposition::decompose_x(
    const Particle& particle, const vector2& r0,
    double n, double q, int ne_x, int ne_y, p_v3f& temp_J) {
  const vector2& r = particle.point.r;

  long int node_x, node_y;
  node_x = ne_x - charge_cloud_;

  for (int x = -charge_cloud_, y = -charge_cloud_; y <= +charge_cloud_; ++y) {
    node_y = ne_y + y;

    // Give up trying to put (node_x + 0.5) here
    temp_J.x(y,x) = - q * n/Np_ * 0.5 * dx / dt *
      (shape_at_(r.x() - node_x * dx, dx) - shape_at_(r0.x() - node_x * dx, dx)) *
      (shape_at_(r.y() - node_y * dy, dy) + shape_at_(r0.y() - node_y * dy, dy));

    #pragma omp atomic
    J_.x(node_y,node_x) += temp_J.x(y,x);
  }

  for (int y = -charge_cloud_; y <= +charge_cloud_; ++y) {
    node_y = ne_y + y;

  for (int x = -charge_cloud_ + 1; x <= +charge_cloud_; ++x) {
    node_x = ne_x + x;

    // Here is only "=" sign (not "+=")
    // Give up trying to put (node_x + 0.5) here
    temp_J.x(y,x) = temp_J.x(y,x-1) - q * n/Np_ * 0.5 * dx / dt *
      (shape_at_(r.x() - node_x * dx, dx) - shape_at_(r0.x() - node_x * dx, dx)) *
      (shape_at_(r.y() - node_y * dy, dy) + shape_at_(r0.y() - node_y * dy, dy));

    #pragma omp atomic
    J_.x(node_y, node_x) += temp_J.x(y,x);
  }}
}


void Esirkepov_density_decomposition::decompose_y(
    const Particle& particle, const vector2& r0,
    double n, double q, int ne_x, int ne_y, p_v3f& temp_J) {
  const vector2& r = particle.point.r;

  long int node_x, node_y;
  node_y = ne_y - charge_cloud_;

  for (int y = -charge_cloud_, x = -charge_cloud_; x <= +charge_cloud_; ++x) {
    node_x = ne_x + x;

    // Give up trying to put (node_y + 0.5) here
    temp_J.y(y,x) = - q * n/Np_ * 0.5 * dy / dt *
      (shape_at_(r.x() - node_x * dx, dx) + shape_at_(r0.x() - node_x * dx, dx)) *
      (shape_at_(r.y() - node_y * dy, dy) - shape_at_(r0.y() - node_y * dy, dy));

    #pragma omp atomic
    J_.y(node_y, node_x) += temp_J.y(y,x);
  }

  for (int y = -charge_cloud_ + 1; y <= +charge_cloud_; ++y) {
    node_y = ne_y + y;

  for (int x = -charge_cloud_; x <= +charge_cloud_; ++x) {
    node_x = ne_x + x;

    // Here is only "=" sign (not "+=")
    // Give up trying to put (node_y + 0.5) here
    temp_J.y(y,x) = temp_J.y(y-1,x) - q * n/Np_ * 0.5 * dy / dt *
      (shape_at_(r.x() - node_x * dx, dx) + shape_at_(r0.x() - node_x * dx, dx)) *
      (shape_at_(r.y() - node_y * dy, dy) - shape_at_(r0.y() - node_y * dy, dy));

    #pragma omp atomic
    J_.y(node_y, node_x) += temp_J.y(y,x);
  }}
}


void Esirkepov_density_decomposition::decompose_z(
    const Particle& particle, const vector2& r0,
    double n, double q, int ne_x, int ne_y, p_v3f& temp_J) {
  double m = particle.m();

  const vector2& r = particle.point.r;
  const vector3& p = particle.point.p;

  double gamma_m = sqrt(m * m + p.dot(p));
  double vz = p.z() / gamma_m;

  long int node_x, node_y;

  for (long int y = -charge_cloud_; y <= +charge_cloud_; ++y) {
    node_y = ne_y + y;

  for (long int x = -charge_cloud_; x <= +charge_cloud_; ++x) {
    node_x = ne_x + x;

    temp_J.z(y,x) = q * n/Np_ * vz *
    (
      shape_at_( r.x() - (node_x + 0.5) * dx, dx) * shape_at_( r.y() - (node_y + 0.5) * dy, dy) / 3. +
      shape_at_(r0.x() - (node_x + 0.5) * dx, dx) * shape_at_( r.y() - (node_y + 0.5) * dy, dy) / 6. +
      shape_at_( r.x() - (node_x + 0.5) * dx, dx) * shape_at_(r0.y() - (node_y + 0.5) * dy, dy) / 6. +
      shape_at_(r0.x() - (node_x + 0.5) * dx, dx) * shape_at_(r0.y() - (node_y + 0.5) * dy, dy) / 3.
    );

    #pragma omp atomic
    J_.z(node_y, node_x) += temp_J.z(y,x);
  }}
}
