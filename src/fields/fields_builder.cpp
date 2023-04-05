#include "fields_builder.hpp"

std::unique_ptr<vector3_field>
Fields_builder::build_field(std::string type) {
  std::unique_ptr<vector3_field> field = nullptr;

  if (config::boundaries.empty()) {
    throw std::runtime_error("Initialization error: No boundaries in file constants.h");
  }
  else if (config::boundaries == "px_py") {
    field = std::make_unique<px_py_vector3_field>(SIZE_X, SIZE_Y);
  }
  else if (config::boundaries == "rx_py") {
    field = std::make_unique<rx_py_vector3_field>(type, SIZE_X, SIZE_Y);
  }
  else if (config::boundaries == "cx_py") {
    field = std::make_unique<cx_py_vector3_field>(SIZE_X, SIZE_Y);
  }
  else if (config::boundaries == "cx_cy") {
    field = std::make_unique<cx_cy_vector3_field>(SIZE_X, SIZE_Y);
  }
  else {
    throw std::runtime_error("Initialization error: No matching boundaries");
  }

  return field;
}

Boundaries_processor_up
Fields_builder::build_boundary_processor(vector3_field& E, vector3_field& B) {
  Damping_layer layer;
  layer.r_beginning = 0.5 * SIZE_X - config::damping_layer_width;
  layer.damping_factor = config::damping_factor;

  return std::make_unique<Open_boundaries_processor>(E, B, layer);
}
