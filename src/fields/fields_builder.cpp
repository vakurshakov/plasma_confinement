#include "fields_builder.hpp"

std::unique_ptr<vector3_field>
Fields_builder::build_field(std::string type) {
  return std::make_unique<vector3_field>(type, SIZE_X, SIZE_Y);
}

Boundaries_processor_up
Fields_builder::build_boundary_processor(vector3_field& E, vector3_field& B) {
  Damping_layer layer;
  layer.r_beginning = 0.5 * SIZE_X - config::damping_layer_width;
  layer.damping_factor = config::damping_factor;

  return std::make_unique<Open_boundaries_processor>(E, B, layer);
}
