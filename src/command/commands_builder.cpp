#include "commands_builder.hpp"

#include "src/utils/configuration.hpp"

#include "src/command/set_fields_distribution.hpp"
#include "src/command/magnetic_field_half_step.hpp"

#include "src/command/set_particles.hpp"
#include "src/command/copy_coordinates.hpp"
#include "src/command/ionize_particles.hpp"
#include "src/command/clone_layer_particles.hpp"

#include "src/particles/particles_load.hpp"

Commands_builder::Commands_builder(
    std::vector<Particles>& particles_species, Fields& fields)
    : fields_(fields), out_dir_(Configuration::out_dir()) {
  for (auto& sort : particles_species) {
    particles_species_.emplace(sort.get_parameters().get_name(), sort);
  }
}

using command_up = std::unique_ptr<Command>;
using list_of_commands = std::list<command_up>;

list_of_commands Commands_builder::build_presets() {
  PROFILE_FUNCTION();
  return build("Presets");
}

list_of_commands Commands_builder::build_step_presets() {
  PROFILE_FUNCTION();
  return build("Step_presets");
}

list_of_commands Commands_builder::build(const std::string& name) {
  PROFILE_FUNCTION();
  LOG_TRACE("Building {}...", name);

  list_of_commands commands;

  if (name == "Step_presets") {
    // We use this to push the magnetic field to
    // the same half-timestep as electric field
    commands.emplace_back(std::make_unique<Magnetic_field_half_step>(&fields_));
  }

  const Configuration& config = Configuration::instance();

  if (!config.contains(name))
    return commands;

  config.for_each(name, [&](const Configuration_item& item) {
    if (item.contains("Set_fields_distribution")) {
      commands.emplace_back(build_set_fields_distribution(item));
    }
    else if (item.contains("Set_particles")) {
      commands.emplace_back(build_set_particles(item));
    }
    else if (item.contains("Copy_coordinates")) {
      // commands.emplace_back(build_copy_coordinates(item))
    }
    else if (item.contains("Ionize_particles")) {
      // commands.emplace_back(build_ionize_particles(item))
    }
    else if (item.contains("Clone_layer_particles")) {
      // commands.emplace_back(build_clone_layer_particles(item))
    }
    else {
      throw std::runtime_error("Unknown command for setup in config file!");
    }
  });

  return commands;
}

command_up
Commands_builder::build_set_fields_distribution(const Configuration_item& item) {
  static const std::string command_name = "Set_fields_distribution";

  if (!item.contains(command_name + ".E") &&
      !item.contains(command_name + ".B"))
    throw std::runtime_error("Initialization error: Neither E nor B is "
      "present in " + command_name + " description");

  vector3 E0, B0;
  if (item.contains(command_name + ".E")) {
    E0.x() = item.get(command_name + ".E.x", 0.0);
    E0.y() = item.get(command_name + ".E.y", 0.0);
    E0.z() = item.get(command_name + ".E.z", 0.0);
  }

  if (item.contains(command_name + ".B")) {
    B0.x() = item.get(command_name + ".B.x", 0.0);
    B0.y() = item.get(command_name + ".B.y", 0.0);
    B0.z() = item.get(command_name + ".B.z", 0.0);
  }

  if (E0 == vector3::zero && B0 == vector3::zero)
    throw std::runtime_error("Initialization error: Both E0 and B0 "
      "are zero for " + command_name);

  return std::make_unique<Set_fields_distribution>(
    &fields_, E0, B0);
}

command_up
Commands_builder::build_set_particles(const Configuration_item& item) {
  static const std::string command_name = "Set_particles";

  std::string particles_name = item.get(command_name);
  Particles* const particles = &particles_species_.at(particles_name);

  Domain_geometry circumscribing_rectangle;
  circumscribing_rectangle.x_min = item.get<double>("Circumscribing_rectangle.x_min", 0.0);
  circumscribing_rectangle.y_min = item.get<double>("Circumscribing_rectangle.y_min", 0.0);
  circumscribing_rectangle.x_max = item.get<double>("Circumscribing_rectangle.x_max", SIZE_X * dx);
  circumscribing_rectangle.y_max = item.get<double>("Circumscribing_rectangle.y_max", SIZE_Y * dx);

  Set_particles::Filling_condition filling_condition;
  if (item.contains("Fill_rectangle")) {
    filling_condition = Fill_rectangle{
      item.get<double>("Fill_rectangle.x_min", 0.0),
      item.get<double>("Fill_rectangle.x_max", SIZE_X * dx),
      item.get<double>("Fill_rectangle.y_min", 0.0),
      item.get<double>("Fill_rectangle.y_max", SIZE_Y * dy),
    };
  }
  else if (item.contains("Fill_circle")) {
    double r = item.get<double>("Fill_circle.r");
    filling_condition = Fill_annulus{
      r,
      item.get<double>("Fill_circle.center_x", 0.5 * SIZE_X * dx),
      item.get<double>("Fill_circle.center_y", 0.5 * SIZE_Y * dy),
    };
  }
  else if (item.contains("Fill_annulus")) {
    double inner_radius = item.get<double>("Fill_annulus.inner_radius");
    double outer_radius = item.get<double>("Fill_annulus.outer_radius");
    filling_condition = Fill_annulus{
      inner_radius,
      outer_radius,
      item.get<double>("Fill_annulus.center_x", 0.5 * SIZE_X * dx),
      item.get<double>("Fill_annulus.center_y", 0.5 * SIZE_Y * dy),
    };
  }
  else {
    LOG_WARN("Filling_condition not specified or unknown for {}, using the "
      "default one: Fill_rectangle(0, size_x, 0, size_y)", particles_name);

    filling_condition = Fill_rectangle{0.0, SIZE_X * dx, 0.0, SIZE_Y * dy};
  }

  Set_particles::Density_profile density_profile;
  if (item.contains("Uniform_profile")) {
    density_profile = uniform_profile;
  }
  else if (item.contains("Cosine_r_profile")) {
    double cos_width = item.get<double>("Cosine_r_profile.cos_width");
    double cos_center = item.get<double>("Cosine_r_profile.cos_center");
    density_profile = Cosine_r_profile{
      cos_width,
      cos_center,
      item.get<double>("Cosine_r_profile.center_x", 0.5 * SIZE_X * dx),
      item.get<double>("Cosine_r_profile.center_y", 0.5 * SIZE_Y * dy),
    };
  }
  else {
    LOG_WARN("Density_profile not specified or unknown for {}, using the "
      "default one: uniform_profile", particles_name);

    density_profile = uniform_profile;
  }

  // No diversity currently
  Set_particles::Coordinate_generator coordinate_generator = load_randomly;

  Set_particles::Momentum_generator momentum_generator;
  if (item.contains("Load_maxwellian_momentum")) {
    momentum_generator = Load_maxwellian_momentum{particles->get_parameters()};
  }
  else if (item.contains("Load_angular_momentum")) {
    momentum_generator = Load_angular_momentum{
      particles->get_parameters(),
      item.get<double>("Load_angular_momentum.center_x", 0.5 * SIZE_X * dx),
      item.get<double>("Load_angular_momentum.center_y", 0.5 * SIZE_Y * dy),
    };
  }
  else {
    LOG_WARN("Momentum_generator not specified or unknown for {}, using the "
      "default one: Load_maxwellian_momentum", particles_name);

    momentum_generator = Load_maxwellian_momentum{particles->get_parameters()};
  }

  return std::make_unique<Set_particles>(
    particles,
    circumscribing_rectangle,
    filling_condition,
    density_profile,
    coordinate_generator,
    momentum_generator
  );
}
