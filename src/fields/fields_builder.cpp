#include "fields_builder.hpp"

#include "src/diagnostics/energy.hpp"
#include "src/diagnostics/field_along_the_line.hpp"
#include "src/diagnostics/field_at_point.hpp"
#include "src/diagnostics/whole_field.hpp"

std::unique_ptr<vector3_field>
Fields_builder::build_field(string type) {
  std::unique_ptr<vector3_field> field = nullptr;

  if (config::boundaries.empty()) {
    throw std::runtime_error("Initialization error: No boundaries in file constants.h");
  }
  else if (config::boundaries == "px_py") {
    field = std::make_unique<px_py_vector3_field>(SIZE_X, SIZE_Y);
  }
  else if (config::boundaries == "rx_ry") {
    field = std::make_unique<rx_ry_vector3_field>(type, SIZE_X, SIZE_Y);
  }
  else {
    throw std::runtime_error("Initialization error: No matching boundaries");
  }

  return field;
}

Boundaries_processor_up
Fields_builder::build_boundary_processor(vector3_field& E, vector3_field& B) {
  Damping_layer layer;
  layer.width = config::damping_layer_width;
  layer.damping_factor = config::damping_factor;

  return std::make_unique<Open_boundaries_processor>(E, B, layer);
}

vector<std::unique_ptr<Fields_diagnostic>>
Fields_builder::build_diagnostics() {
  vector<std::unique_ptr<Fields_diagnostic>> vec_diagnostics;

#if there_are_fields && fields_are_diagnosed
  if (config::fields_diagnostics.empty())
    throw std::runtime_error("Initialization error: fields are diagnosed but no fields_diagnostics in file constants.h");

  for (const auto& [name, description] : config::fields_diagnostics) {
    if ( name == "energy" ) {
      vec_diagnostics.emplace_back(std::make_unique<fields_energy>(dir_name + "/" + name));
    }
    else if (name == "whole_field") {
      vec_diagnostics.emplace_back(std::make_unique<whole_field>(
        dir_name + "/fields/" + name,
        description[0] + description[1], description[0], description[1],
        std::stoi(description[2]), std::stoi(description[3]),
        std::stoi(description[4]), std::stoi(description[5])));
    }
    else if (name == "field_along_x_axis") {
      vec_diagnostics.emplace_back(std::make_unique<field_along_x_axis>(
        dir_name + "/fields/" + name,
        description[0] + description[1], description[0], description[1],
        std::stoi(description[2]),
        std::stoi(description[3]),
        std::stoi(description[4])));
    }
    else if (name == "field_along_y_axis") {
      vec_diagnostics.emplace_back(std::make_unique<field_along_y_axis>(
        dir_name + "/fields/" + name,
        description[0] + description[1], description[0], description[1],
        std::stoi(description[2]),
        std::stoi(description[3]),
        std::stoi(description[4])));
    }
    else if ( name == "field_at_point" ) {
      vec_diagnostics.emplace_back(std::make_unique<field_at_point>(
        dir_name + "/fields/" + name,
        description[0] + description[1], description[0], description[1],
        std::stoi(description[2]),
        std::stoi(description[3])));
    }
    else {
      throw std::runtime_error("Initialization error: No matching diagnostics for fields.");
    }
  }
  vec_diagnostics.shrink_to_fit();

#endif
  return vec_diagnostics;
}
