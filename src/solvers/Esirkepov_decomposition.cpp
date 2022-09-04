#include "Esirkepov_decomposition.hpp"

#include "src/particles/sort_common.hpp"
#include "src/vectors/grid_indexes.hpp"

namespace regular {

Esirkepov_decomposition::Esirkepov_decomposition(
    const Particles2D3V& particles, electric_field& J)
    : particles(particles),
      fields_current(J) {}

void Esirkepov_decomposition::process(
    Particles::Particle_id i, const vector2& r0) {
  double particle_const = particles.q(i) *
    particles.n(i) / particles.get_parameters().Np;

  decompose_Jx(i, r0, particle_const);
  decompose_Jy(i, r0, particle_const);
  decompose_Jz(i, r0, particle_const);
}

void Esirkepov_decomposition::decompose_Jx(
    Particles::Particle_id i, const vector2& r0, double particle_const) {
  double xi = particles.coordinate.x(i);
  double yi = particles.coordinate.y(i);

  spline_shape* shape = particles.get_parameters().shape.get();
  int shape_radius = shape->radius;

  grid_indexes::index nearest{
    int(round(xi / dx)),
    int(round(yi / dy)),
  };

  grid_indexes::index g;
  g.x = nearest.x - shape_radius;

  basic_field<1> temp_current(2 * shape_radius + 1);

  for (int x = -shape_radius, y = -shape_radius; y <= +shape_radius; ++y) {
    g.y = nearest.y + y;

    if (!fields_current.is_writable(0, g.x, g.y)) continue;

    g.x += 0.5;  // Jx is shifted along x
    int tx = shape_radius + x;
    int ty = shape_radius + y;
    temp_current(0, tx, ty) = - 0.5 * dx / dt * particle_const *
      (shape->form(xi - g.x * dx, dx) - shape->form(r0.x - g.x * dx, dx)) *
      (shape->form(yi - g.y * dy, dy) + shape->form(r0.y - g.y * dy, dy));

    #pragma omp atomic
    fields_current(0, g.x, g.y) += temp_current(0, tx, ty);
  }

  for (int x = -shape_radius + 1; x <= +shape_radius; ++x) {
    g.x = nearest.x + x;

    for (int y = -shape_radius; y <= +shape_radius; ++y) {
      g.y = nearest.y + y;

      if (!fields_current.is_writable(0, g.x, g.y)) continue;

      g.x += 0.5;
      int tx = shape_radius + x;
      int ty = shape_radius + y;
      temp_current(0, tx, ty) =
        temp_current(0, tx - 1, ty) - 0.5 * dx / dt * particle_const *
        (shape->form(xi - g.x * dx, dx) - shape->form(r0.x - g.x * dx, dx)) *
        (shape->form(yi - g.y * dy, dy) + shape->form(r0.y - g.y * dy, dy));

      #pragma omp atomic
      fields_current(0, g.x, g.y) += temp_current(0, tx, ty);
    }
  }
}

void Esirkepov_decomposition::decompose_Jy(
    Particles::Particle_id i, const vector2& r0, double particle_const) {
  double xi = particles.coordinate.x(i);
  double yi = particles.coordinate.y(i);

  spline_shape* shape = particles.get_parameters().shape.get();
  int shape_radius = shape->radius;

  grid_indexes::index nearest{
    int(round(xi / dx)),
    int(round(yi / dy)),
  };

  grid_indexes::index g;
  g.y = nearest.y - shape_radius;

  basic_field<1> temp_current(2 * shape_radius + 1);

  for (int y = -shape_radius, x = -shape_radius; x <= +shape_radius; ++x) {
    g.x = nearest.x + x;

    if (!fields_current.is_writable(1, g.x, g.y)) continue;

    g.y += 0.5;  // Jy is shifted along y
    int tx = shape_radius + x;
    int ty = shape_radius + y;
    temp_current(0, tx, ty) = - 0.5 * dx / dt * particle_const *
      (shape->form(xi - g.x * dx, dx) + shape->form(r0.x - g.x * dx, dx)) *
      (shape->form(yi - g.y * dy, dy) - shape->form(r0.y - g.y * dy, dy));

    #pragma omp atomic
    fields_current(1, g.x, g.y) += temp_current(0, tx, ty);
  }


  for (int x = -shape_radius; x <= +shape_radius; ++x) {
    g.x = nearest.x + x;

    for (int y = -shape_radius + 1; y <= +shape_radius; ++y) {
      g.y = nearest.y + y;

      if (!fields_current.is_writable(1, g.x, g.y)) continue;

      g.y += 0.5;
      int tx = shape_radius + x;
      int ty = shape_radius + y;
      temp_current(0, tx, ty) =
        temp_current(0, tx, ty - 1) - 0.5 * dx / dt * particle_const *
        (shape->form(xi - g.x * dx, dx) + shape->form(r0.x - g.x * dx, dx)) *
        (shape->form(yi - g.y * dy, dy) - shape->form(r0.y - g.y * dy, dy));

      #pragma omp atomic
      fields_current(1, g.x, g.y) += temp_current(0, tx, ty);
   }
  }
}

void Esirkepov_decomposition::decompose_Jz(
    Particles::Particle_id i, const vector2& r0, double particle_const) {
  double xi = particles.coordinate.x(i);
  double yi = particles.coordinate.y(i);
  double vz = particles.momentum.get_vz(i, particles.m(i));

  spline_shape* shape = particles.get_parameters().shape.get();
  int shape_radius = shape->radius;

  grid_indexes::index nearest{
    int(round(xi / dx)),
    int(round(yi / dy)),
  };

  grid_indexes::index g;

  basic_field<1> temp_current(2 * shape_radius + 1);

  for (long int x = -shape_radius; x <= +shape_radius; ++x) {
    g.x = nearest.x + x;

    for (long int y = -shape_radius; y <= +shape_radius; ++y) {
      g.y = nearest.y + y;

      if (!fields_current.is_writable(2, g.x, g.y)) continue;

      int tx = shape_radius + x;
      int ty = shape_radius + y;
      temp_current(0, tx, ty) = vz * particle_const * (
        shape->form(xi   - g.x * dx, dx) * shape->form(yi   - g.y * dy, dy) / 3. +
        shape->form(r0.x - g.x * dx, dx) * shape->form(yi   - g.y * dy, dy) / 6. +
        shape->form(xi   - g.x * dx, dx) * shape->form(r0.y - g.y * dy, dy) / 6. +
        shape->form(r0.x - g.x * dx, dx) * shape->form(r0.y - g.y * dy, dy) / 3.
      );

      #pragma omp atomic
      fields_current(2, g.x, g.y) += temp_current(0, tx, ty);
    }
  }
}

}  // namespace regular
