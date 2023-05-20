#ifndef SRC_DIAGNOSTICS_DIAGNOSTICS_BUILDER_HPP
#define SRC_DIAGNOSTICS_DIAGNOSTICS_BUILDER_HPP

#include "diagnostic.hpp"

#include "src/pch.h"
#include "src/particles/particles.hpp"
#include "src/fields/fields.hpp"
#include "src/utils/configuration.hpp"

class Diagnostics_builder {
 public:
  Diagnostics_builder(std::vector<Particles>& particles_species, Fields& fields);

  std::vector<std::unique_ptr<Diagnostic>> build();

 private:
  std::map<std::string, const Particles&> particles_species_;
  Fields& fields_;
  std::string out_dir_;

  using diagnostic_up = std::unique_ptr<Diagnostic>;
  using vector_of_diagnostics = std::vector<diagnostic_up>;

  diagnostic_up build_fields_energy();

  void build_field_at_point(const Configuration_item& description,
    vector_of_diagnostics& diagnostics_container);

  void build_field_on_segment(const Configuration_item& description,
    vector_of_diagnostics& diagnostics_container);

  void build_whole_field(const Configuration_item& description,
    vector_of_diagnostics& diagnostics_container);

  struct Field_description {
    std::pair<std::string, const vector3_field*> field;
    std::map<std::string, Axis> components;
  };
  Field_description create_field_description(const Configuration_item& description);
  std::list<Field_description> collect_field_descriptions(
    const Configuration_item& description, const std::string& diag_name);


  diagnostic_up
  build_particles_energy(const std::string& sort_name);

  diagnostic_up
  build_distribution_moment(
    const std::string& sort_name,
    const std::string& moment_name,
    const std::string& axes_names,
    const std::vector<std::string>& description);

  /// add integration region for moment distribution
  // diagnostic_up
  // build_x0_distribution_function(
  //   const std::string& sort_name,
  //   const std::vector<std::string>& description);
};

#endif  // SRC_DIAGNOSTICS_DIAGNOSTICS_BUILDER_HPP
