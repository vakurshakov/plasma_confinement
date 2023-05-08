#ifndef SRC_COMMAND_COMMAND_BUILDER_HPP
#define SRC_COMMAND_COMMAND_BUILDER_HPP

#include "command.hpp"

#include "src/pch.h"
#include "src/fields/fields.hpp"
#include "src/particles/particles.hpp"
#include "src/utils/configuration.hpp"

class Commands_builder {
 public:
  Commands_builder(std::vector<Particles>& particles_species, Fields& fields);

  std::list<std::unique_ptr<Command>> build_presets();
  std::list<std::unique_ptr<Command>> build_step_presets();

 private:
  std::map<std::string, Particles&> particles_species_;
  Fields& fields_;
  std::string out_dir_;

  using command_up = std::unique_ptr<Command>;
  std::list<command_up> build(const std::string& name);

  command_up build_set_fields_distribution(const Configuration_item& item);
  command_up build_set_particles(const Configuration_item& item);
  command_up build_copy_coordinates(const Configuration_item& item);
  command_up build_ionize_particles(const Configuration_item& item);
  // command_up build_clone_layer_particles(const Configuration_item& item);
};

#endif // SRC_COMMAND_COMMAND_BUILDER_HPP
