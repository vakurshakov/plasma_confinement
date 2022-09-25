#ifndef SRC_DIAGNOSTICS_DIAGNOSTICS_BUILDER_HPP
#define SRC_DIAGNOSTICS_DIAGNOSTICS_BUILDER_HPP

#include "diagnostic.hpp"

#include "src/pch.h"
#include "src/particles/particles.hpp"
#include "src/fields/fields.hpp"

/// @todo description length check to pass predefined values
class Diagnostics_builder {
 public:
  Diagnostics_builder(
    std::vector<Particles>& particles_species,
    Fields& fields);

  std::vector<std::unique_ptr<Diagnostic>> build();

 private:
  std::map<std::string, const Particles&> particles_species_;
  Fields& fields_;

  inline const vector3_field& get_field(const std::string& name);
  inline Axis get_component(const std::string& component);

#define BUILD_FIELD_DIAG(diag)                                    \
  inline std::unique_ptr<Diagnostic>                              \
  build_diag_##diag(const std::vector<std::string>& description)  \

  BUILD_FIELD_DIAG(fields_energy);
  BUILD_FIELD_DIAG(field_at_point);
  BUILD_FIELD_DIAG(field_on_segment);
  BUILD_FIELD_DIAG(whole_field);

#undef BUILD_FIELD_DIAG

  inline std::unique_ptr<Diagnostic>
  build_diag_particles_energy(const std::string& sort_name);

  inline std::unique_ptr<Diagnostic>
  build_diag_distribution_moment(
    const std::string& sort_name,
    const std::string& moment_name,
    const std::string& axes_names,
    const std::vector<std::string>& description);
};

#endif  // SRC_DIAGNOSTICS_DIAGNOSTICS_BUILDER_HPP
