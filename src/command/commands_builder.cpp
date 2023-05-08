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
      commands.emplace_back(build_copy_coordinates(item));
    }
    else if (item.contains("Ionize_particles")) {
      commands.emplace_back(build_ionize_particles(item));
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

std::function<vector3(const vector2& r)>
build_momentum_generator(Particles* const particles, const Configuration_item& item) {
  const Parameters& parameters = particles->get_parameters();

  std::function<vector3(const vector2& r)> momentum_generator;
  if (item.contains("Load_maxwellian_momentum")) {
    momentum_generator = Load_maxwellian_momentum{parameters};
  }
  else if (item.contains("Load_angular_momentum")) {
    momentum_generator = Load_angular_momentum{
      parameters,
      item.get<double>("Load_angular_momentum.center_x", 0.5 * SIZE_X * dx),
      item.get<double>("Load_angular_momentum.center_y", 0.5 * SIZE_Y * dy),
    };
  }
  else {
    LOG_WARN("Momentum_generator not specified or unknown for {}, using the "
      "default one: Load_maxwellian_momentum.", parameters.get_name());

    momentum_generator = Load_maxwellian_momentum{parameters};
  }
  return momentum_generator;
}

std::function<double(int nx, int ny)>
build_density_profile(const Configuration_item& item) {
  std::function<double(int nx, int ny)> density_profile;
  if (item.contains("Uniform_profile")) {
    density_profile = uniform_profile;
  }
  else if (item.contains("Cosine_r_profile")) {
    double cos_width = item.get<double>("Cosine_r_profile.width");
    double cos_center = item.get<double>("Cosine_r_profile.center");
    density_profile = Cosine_r_profile{
      cos_width,
      cos_center,
      item.get<double>("Cosine_r_profile.center_x", 0.5 * SIZE_X * dx),
      item.get<double>("Cosine_r_profile.center_y", 0.5 * SIZE_Y * dy),
    };
  }
  else {
    LOG_WARN("Density_profile not specified or unknown, "
      "using the default one: uniform_profile.");

    density_profile = uniform_profile;
  }
  return density_profile;
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
    double r = item.get<double>("Fill_circle.radius");
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
      "default one: Fill_rectangle(0, size_x, 0, size_y).", particles_name);

    filling_condition = Fill_rectangle{0.0, SIZE_X * dx, 0.0, SIZE_Y * dy};
  }

  Set_particles::Density_profile density_profile = build_density_profile(item);

  // No diversity currently
  Set_particles::Coordinate_generator coordinate_generator = load_randomly;

  Set_particles::Momentum_generator momentum_generator =
    build_momentum_generator(particles, item);

  return std::make_unique<Set_particles>(
    particles,
    circumscribing_rectangle,
    filling_condition,
    density_profile,
    coordinate_generator,
    momentum_generator
  );
}

command_up
Commands_builder::build_copy_coordinates(const Configuration_item& item) {
  static const std::string command_name = "Copy_coordinates";

  Particles* const particles_copy_to = &particles_species_.at(
    item.get(command_name + ".to"));

  Particles* const particles_copy_from = &particles_species_.at(
    item.get(command_name + ".from"));

  Copy_coordinates::Momentum_generator momentum_generator =
    build_momentum_generator(particles_copy_to, item);

  return std::make_unique<Copy_coordinates>(
    particles_copy_to,
    particles_copy_from,
    momentum_generator
  );
}

command_up
Commands_builder::build_ionize_particles(const Configuration_item& item) {
  static const std::string command_name = "Ionize_particles";

  Particles* const particles_ionized = &particles_species_.at(
    item.get(command_name + ".ionized"));

  Particles* const particles_ejected = &particles_species_.at(
    item.get(command_name + ".ejected"));

  int start = item.get<int>(command_name + ".start", 0);
  int end = item.get<int>(command_name + ".end", TIME);
  int rate = item.get<int>(command_name + ".rate");

  Ionize_particles::Random_coordinate_generator coordinate_generator;
  if (item.contains("On_rectangle")) {
    coordinate_generator = Coordinate_on_rectangle{
      item.get<double>("On_rectangle.x_min", 0.0),
      item.get<double>("On_rectangle.x_max", SIZE_X * dx),
      item.get<double>("On_rectangle.y_min", 0.0),
      item.get<double>("On_rectangle.y_max", SIZE_Y * dy),
    };
  }
  else if (item.contains("On_circle")) {
    double r = item.get<double>("On_circle.radius");
    coordinate_generator = Coordinate_on_circle{
      r,
      item.get<double>("On_circle.center_x", 0.5 * SIZE_X * dx),
      item.get<double>("On_circle.center_y", 0.5 * SIZE_Y * dy),
    };
  }
  else if (item.contains("On_annulus")) {
    double inner_radius = item.get<double>("On_annulus.inner_radius");
    double outer_radius = item.get<double>("On_annulus.outer_radius");
    coordinate_generator = Coordinate_on_annulus{
      inner_radius,
      outer_radius,
      item.get<double>("On_annulus.center_x", 0.5 * SIZE_X * dx),
      item.get<double>("On_annulus.center_y", 0.5 * SIZE_Y * dy),
    };
  }
  else {
    LOG_WARN("Random_coordinate_generator not specified or unknown, using the "
      "default one: Coordinate_on_rectangle(0, size_x, 0, size_y).");

    coordinate_generator = Coordinate_on_rectangle{0.0, SIZE_X * dx, 0.0, SIZE_Y * dy};
  }

  Ionize_particles::Density_profile density_profile = build_density_profile(item);

  /// @todo Build generators for each particles specie?
  Ionize_particles::Momentum_generator i_momentum_generator =
    build_momentum_generator(particles_ionized, item);

  Ionize_particles::Momentum_generator e_momentum_generator =
    build_momentum_generator(particles_ejected, item);

  return std::make_unique<Ionize_particles>(
    particles_ionized,
    particles_ejected,
    start,
    end,
    rate,
    coordinate_generator,
    density_profile,
    i_momentum_generator,
    e_momentum_generator
  );
}
