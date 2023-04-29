#include "open_boundaries_processor.hpp"

double Damping_layer::damping_coeff(double r) {
  static const double width = 0.5 * SIZE_X - r_beginning;
  static const double dr_0 = width * (1.0 + 1.0 / sqrt(damping_factor));

  double dr = r - r_beginning;

  if (dr < dr_0)
    return 1.0 - damping_factor * (dr / width - 1.0) * (dr / width - 1.0);

  return 0.0;
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
  top_bottom_bounds();
}

void Open_boundaries_processor::left_right_bounds() {
  static const double center_x = 0.5 * SIZE_X;
  static const double center_y = 0.5 * SIZE_Y;
  static const int inner_min = int(layer.r_beginning / M_SQRT2);

  #pragma omp parallel for num_threads(OMP_NUM_THREADS)
  for (int y = 0; y < SIZE_Y; ++y) {
  for (int x = 0; x <= inner_min; ++x) {

    double r = sqrt((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y));
    if (r < layer.r_beginning)
      continue;

    // left
    double coeff = layer.damping_coeff(r);

    fields_E.x(y, x) *= coeff;
    fields_E.y(y, x) *= coeff;
    fields_B.z(y, x) = fields_B.z(y, x) * coeff + config::Omega_max * (1.0 - coeff);

    // right
    int right_x = (SIZE_X - 1) - x;

    fields_E.x(y, right_x) *= coeff;
    fields_E.y(y, right_x) *= coeff;
    fields_B.z(y, right_x) = fields_B.z(y, right_x) * coeff + config::Omega_max * (1.0 - coeff);

  #if _2D3V
    fields_E.z(y, x) *= coeff;
    fields_B.x(y, x) *= coeff;
    fields_B.y(y, x) *= coeff;

    fields_E.z(y, right_x) *= coeff;
    fields_B.x(y, right_x) *= coeff;
    fields_B.y(y, right_x) *= coeff;
  #endif
  }}
}

void Open_boundaries_processor::top_bottom_bounds() {
  static const double center_x = 0.5 * SIZE_X;
  static const double center_y = 0.5 * SIZE_Y;
  static const int inner_min = int(layer.r_beginning / M_SQRT2);

  #pragma omp parallel for num_threads(OMP_NUM_THREADS)
  for (int x = inner_min; x < (SIZE_X - 1) - inner_min; ++x) {
  for (int y = 0; y <= inner_min; ++y) {

    double r = sqrt((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y));
    if (r < layer.r_beginning)
      continue;

    // bottom
    double coeff = layer.damping_coeff(r);

    fields_E.x(y, x) *= coeff;
    fields_E.y(y, x) *= coeff;
    fields_B.z(y, x) = fields_B.z(y, x) * coeff + config::Omega_max * (1.0 - coeff);

    // top
    int top_y = (SIZE_Y - 1) - y;

    fields_E.x(top_y, x) *= coeff;
    fields_E.y(top_y, x) *= coeff;
    fields_B.z(top_y, x) = fields_B.z(top_y, x) * coeff + config::Omega_max * (1.0 - coeff);

  #if _2D3V
    fields_E.z(y, x) *= coeff;
    fields_B.x(y, x) *= coeff;
    fields_B.y(y, x) *= coeff;

    fields_E.z(y, right_x) *= coeff;
    fields_B.x(y, right_x) *= coeff;
    fields_B.y(y, right_x) *= coeff;
  #endif
  }}
}
