#include "commands_builder.hpp"

#include "src/utils/configuration.hpp"

#include "src/command/set_fields_distribution.hpp"
#include "src/command/magnetic_field_half_step.hpp"

#include "src/command/set_particles.hpp"
#include "src/command/copy_coordinates.hpp"
#include "src/command/ionize_particles.hpp"
#include "src/command/clone_layer_particles.hpp"

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
  LOG_TRACE("Building commands...");

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
      // commands.emplace_back(build_set_particles(item))
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
      !item.contains(command_name + ".B")) {
    throw std::runtime_error("Initialization error: Neither E nor B is present "
      "in " + command_name + " description");
  }

  LOG_INFO("Add uniform field distribution to setup, with:");
  vector3 electric_field;
  vector3 magnetic_field;

  if (item.contains(command_name + ".E")) {
    electric_field.x() = item.get(command_name + ".E.x", 0.0);
    electric_field.y() = item.get(command_name + ".E.y", 0.0);
    electric_field.z() = item.get(command_name + ".E.z", 0.0);

    LOG_INFO("  Electric field = ({:2e}, {:2e}, {:2e})",
      electric_field.x(), electric_field.y(), electric_field.z());
  }

  if (item.contains(command_name + ".B")) {
    magnetic_field.x() = item.get(command_name + ".B.x", 0.0);
    magnetic_field.y() = item.get(command_name + ".B.y", 0.0);
    magnetic_field.z() = item.get(command_name + ".B.z", 0.0);

    LOG_INFO("  Magnetic field = ({:2e}, {:2e}, {:2e})",
      magnetic_field.x(), magnetic_field.y(), magnetic_field.z());
  }

  return std::make_unique<Set_fields_distribution>(
    &fields_, electric_field, magnetic_field);
}
