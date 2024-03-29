#include "open_boundaries_processor.hpp"

double Damping_layer::damping_coeff(int x) {
  return 1.0 - damping_factor *
    ((double) x / (double) width - 1.0) *
    ((double) x / (double) width - 1.0);
}


Open_boundaries_processor::Open_boundaries_processor(
  vector3_field& fields_E,
  vector3_field& fields_B,
  Damping_layer layer)
  : fields_E(fields_E),
    fields_B(fields_B),
    layer(layer) {}

void Open_boundaries_processor::process() {
  PROFILE_FUNCTION();

  left_right_bounds();
  // top_bottom_bounds();
}

void Open_boundaries_processor::left_right_bounds() {
  // * . . * >x
  // *     *
  // * . . *
  // vy

  #pragma omp parallel for num_threads(NUM_THREADS)
  for (int y = 0; y < fields_E.size_y(); ++y) {
    for (int x = 0; x < layer.width; ++x) {
      // left
      double coeff = layer.damping_coeff(x);

      fields_E.x(y, x) *= coeff;
      fields_E.y(y, x) *= coeff;
#if !BEAM_INJECTION_SETUP
      fields_B.z(y, x) *= coeff;
#else
      fields_B.z(y, x) = fields_B.z(y, x) * coeff + config::Omega_max * (1.0 - coeff);
#endif

#if _2D3V
      fields_E.z(y, x) *= coeff;
      fields_B.x(y, x) *= coeff;
      fields_B.y(y, x) *= coeff;
#endif

      // right
      int right_x = (fields_E.size_x() - 1) - x;

      fields_E.x(y, right_x) *= coeff;
      fields_E.y(y, right_x) *= coeff;
      fields_B.z(y, right_x) = fields_B.z(y, right_x) * coeff + config::Omega_max * (1.0 - coeff);

#if _2D3V
      fields_E.z(y, right_x) *= coeff;
      fields_B.x(y, right_x) *= coeff;
      fields_B.y(y, right_x) *= coeff;
#endif
    }
  }
}

void Open_boundaries_processor::top_bottom_bounds() {
  // 2 * * 2 >x
  // *     *
  // 2 * * 2
  // vy

  for (int y = 0; y < layer.width; ++y) {
    #pragma omp parallel for num_threads(NUM_THREADS)
    for (int x = 0; x < fields_E.size_x(); ++x) {
      // top
      double coeff = layer.damping_coeff(y);

      fields_E.x(y, x) *= coeff;
      fields_E.y(y, x) *= coeff;
      fields_E.z(y, x) *= coeff;

      fields_B.x(y, x) *= coeff;
      fields_B.y(y, x) *= coeff;
      fields_B.z(y, x) *= coeff;

      // bottom
      int bottom_y = (fields_E.size_y() - 1) - y;

      fields_E.x(bottom_y, x) *= coeff;
      fields_E.y(bottom_y, x) *= coeff;
      fields_E.z(bottom_y, x) *= coeff;

      fields_B.x(bottom_y, x) *= coeff;
      fields_B.y(bottom_y, x) *= coeff;
      fields_B.z(bottom_y, x) *= coeff;
    }
  }
}
