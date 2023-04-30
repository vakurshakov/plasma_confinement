#include "fields_builder.hpp"

#include "src/utils/configuration.hpp"

std::unique_ptr<vector3_field>
Fields_builder::build_field(std::string type) {
  std::unique_ptr<vector3_field> field = nullptr;

  std::string boundaries = Configuration::instance().get("Geometry.boundaries");
  if (boundaries.empty()) {
    throw std::runtime_error("Initialization error: No boundaries in file constants.h");
  }
  else if (boundaries == "px_py") {
    field = std::make_unique<px_py_vector3_field>(SIZE_X, SIZE_Y);
  }
  else if (boundaries == "rx_py") {
    field = std::make_unique<rx_py_vector3_field>(type, SIZE_X, SIZE_Y);
  }
  else if (boundaries == "cx_py") {
    field = std::make_unique<cx_py_vector3_field>(SIZE_X, SIZE_Y);
  }
  else if (boundaries == "cx_cy") {
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

  const Configuration& config = Configuration::instance();

  layer.r_beginning = 0.5 * SIZE_X - config.get<double>("Damping.layer_width");
  layer.damping_factor = config.get<double>("Damping.factor");

  return std::make_unique<Open_boundaries_processor>(E, B, layer);
}
