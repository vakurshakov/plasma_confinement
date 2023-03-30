#include "diagnostics_builder.hpp"

#include "src/diagnostics/energy.hpp"
#include "src/diagnostics/field_at_point.hpp"
#include "src/diagnostics/field_on_segment.hpp"
#include "src/diagnostics/whole_field.hpp"

#include "src/diagnostics/chosen_particles.hpp"
#include "src/diagnostics/distribution_moment.hpp"
#include "src/diagnostics/x0_distribution_function.hpp"

Diagnostics_builder::Diagnostics_builder(
    std::vector<Particles>& particles_species, Fields& fields)
    : fields_(fields) {
  for (auto& sort : particles_species) {
    particles_species_.emplace(sort.get_parameters().get_name(), sort);
  }
}

using vector_of_diagnostics = std::vector<std::unique_ptr<Diagnostic>>;

vector_of_diagnostics Diagnostics_builder::build() {
  PROFILE_FUNCTION();
  LOG_TRACE("Building diagnostics...");

  vector_of_diagnostics diagnostics;

#if there_are_fields && fields_are_diagnosed
  if (config::fields_diagnostics.empty())
    throw std::runtime_error("Initialization error: fields are diagnosed but no fields_diagnostics in file constants.h");

  for (const auto& [name, description] : config::fields_diagnostics) {
    if (name == "energy") {
      LOG_INFO("Add fields energy diagnostic");
      diagnostics.emplace_back(build_diag_fields_energy(description));
    }
    else if (name == "field_at_point") {
      LOG_INFO("Add field_at_point diagnostic for {}", description[0] + description[1]);
      diagnostics.emplace_back(build_diag_field_at_point(description));
    }
    else if (name == "field_on_segment") {
      LOG_INFO("Add field_on_segment diagnostic for {}", description[0] + description[1]);
      diagnostics.emplace_back(build_diag_field_on_segment(description));
    }
    else if (name == "whole_field") {
      LOG_INFO("Add whole_field diagnostic for {}", description[0] + description[1]);
      diagnostics.emplace_back(build_diag_whole_field(description));
    }
  }
#endif

#if there_are_particles && particles_are_diagnosed
  if (config::species_description.empty())
    throw std::runtime_error("Initialization error: particles are diagnosed but no species_description in file constants.h");

  for (const auto& [sort, sort_description] : config::species_description) {
  for (const auto& [diag, diag_description] : sort_description) {
    if (diag == "energy") {
      LOG_INFO("Add {} energy diagnostics", sort);
      diagnostics.emplace_back(build_diag_particles_energy(sort));
    }
    else if (diag == "density") {
      LOG_INFO("Add density diagnostic for {}", sort);
      diagnostics.emplace_back(build_diag_distribution_moment(
        sort, "zeroth_moment", "XY", diag_description));
    }
    else if (diag == "velocity_distribution") {
      LOG_INFO("Add velocity_distribution diagnostic for {}", sort);
      diagnostics.emplace_back(build_diag_distribution_moment(
        sort, "zeroth_moment", "VxVy", diag_description));
    }
    else if (diag == "Vx_moment") {
      LOG_INFO("Add Vx_moment diagnostic for {}", sort);
      diagnostics.emplace_back(build_diag_distribution_moment(
        sort, "Vx_moment", "XY", diag_description));
    }
    else if (diag == "Vy_moment") {
      LOG_INFO("Add Vy_moment diagnostic for {}", sort);
      diagnostics.emplace_back(build_diag_distribution_moment(
        sort, "Vy_moment", "XY", diag_description));
    }
    else if (diag == "Vr_moment") {
      LOG_INFO("Add Vr_moment diagnostic for {}", sort);
      diagnostics.emplace_back(build_diag_distribution_moment(
        sort, "Vr_moment", "XY", diag_description));
    }
    else if (diag == "Vphi_moment") {
      LOG_INFO("Add Vphi_moment diagnostic for {}", sort);
      diagnostics.emplace_back(build_diag_distribution_moment(
        sort, "Vphi_moment", "XY", diag_description));
    }
    else if (diag == "mVxVx_moment") {
      LOG_INFO("Add mVxVx_moment diagnostic for {}", sort);
      diagnostics.emplace_back(build_diag_distribution_moment(
        sort, "mVxVx_moment", "XY", diag_description));
    }
    else if (diag == "mVxVy_moment") {
      LOG_INFO("Add mVxVy_moment diagnostic for {}", sort);
      diagnostics.emplace_back(build_diag_distribution_moment(
        sort, "mVxVy_moment", "XY", diag_description));
    }
    else if (diag == "mVyVy_moment") {
      LOG_INFO("Add mVyVy_moment diagnostic for {}", sort);
      diagnostics.emplace_back(build_diag_distribution_moment(
        sort, "mVyVy_moment", "XY", diag_description));
    }
    else if (diag == "x0_distribution_function") {
      LOG_INFO("Add x0_distribution_function diagnostic for {}", sort);
      diagnostics.emplace_back(build_diag_x0_distribution_function(
        sort, diag_description));
    }
  }}
#endif

  diagnostics.shrink_to_fit();
  return diagnostics;
}


inline const vector3_field&
Diagnostics_builder::get_field(const std::string& name) {
  if      (name == "E") { return fields_.E(); }
  else if (name == "B") { return fields_.B(); }
  else if (name == "J") { return fields_.J(); }
  else
    throw std::runtime_error("Incorrect field name in diagnostic description");
}

inline Axis
Diagnostics_builder::get_component(const std::string& component) {
  if      (component == "x") { return Axis::X; }
  else if (component == "y") { return Axis::Y; }
  else if (component == "z") { return Axis::Z; }
  else
    throw std::runtime_error("Incorrect field component specified in diagnostic description");
}


#define BUILD_FIELD_DIAG(diag)                    \
  inline std::unique_ptr<Diagnostic>              \
  Diagnostics_builder::build_diag_##diag(         \
    const std::vector<std::string>& description)  \

BUILD_FIELD_DIAG(fields_energy) {
  return std::make_unique<fields_energy>(
    dir_name + "/", fields_.E(), fields_.B());
}

BUILD_FIELD_DIAG(field_at_point) {
  if (description.size() < 2)
    throw std::runtime_error("Initialization error: No point specified on field_at_point diagnostic for " + description[0] + description[1]);

  diag_point point{
    std::stoi(description[2]),
    std::stoi(description[3])
  };

  if ((0 > point.x || point.x >= SIZE_X) ||
      (0 > point.y || point.y >= SIZE_Y)) {
    throw std::runtime_error("Initialization error: Invalid point for field_at_point diagnostic on " + description[0] + description[1] + " is set");
  }

  return std::make_unique<field_at_point>(
    dir_name + "/" + description[0] + description[1]+ "/",
    "point_(" + description[2] + "," + description[3] + ")",
    get_field(description[0]),
    get_component(description[1]),
    point);
}

BUILD_FIELD_DIAG(field_on_segment) {
  if (description.size() < 2)
    throw std::runtime_error("Initialization error: No segment specified for " + description[0] + description[1] + " diagnostic");

  diag_segment segment{
    std::stoi(description[2]), std::stoi(description[3]),
    std::stoi(description[4]), std::stoi(description[5])
  };

  if ((0 > segment.begin[X] || segment.begin[X] > SIZE_X) ||
      (0 > segment.begin[Y] || segment.begin[Y] > SIZE_Y) ||
      (0 > segment.end[X]   || segment.end[X]   > SIZE_X) ||
      (0 > segment.end[Y]   || segment.end[Y]   > SIZE_Y)) {
    throw std::runtime_error("Initialization error: Invalid segment values for " + description[0] + description[1] + " is set");
  }

  return std::make_unique<field_on_segment>(
    dir_name + "/" + description[0] + description[1] + "/segment_(" +
      description[2] + "," + description[3] + ")_(" +
      description[4] + "," + description[5] + ")/",
    get_field(description[0]),
    get_component(description[1]),
    segment);
}

BUILD_FIELD_DIAG(whole_field) {
  if (description.size() < 2)
    throw std::runtime_error("Initialization error: No area specified for " + description[0] + description[1] + " whole_field diagnostic");

  diag_segment area{
    std::stoi(description[2]), std::stoi(description[3]),
    std::stoi(description[4]), std::stoi(description[5])
  };

  if ((0 > area.begin[X] || area.begin[X] > SIZE_X) ||
      (0 > area.begin[Y] || area.begin[Y] > SIZE_Y) ||
      (0 > area.end[X]   || area.end[X]   > SIZE_X) ||
      (0 > area.end[Y]   || area.end[Y]   > SIZE_Y)) {
    throw std::runtime_error("Initialization error: Invalid area values for " + description[0] + description[1] + " whole_field diagnostic is set");
  }

  return std::make_unique<whole_field>(
    dir_name + "/" + description[0] + description[1] + "/whole_field/",
    get_field(description[0]),
    get_component(description[1]),
    diag_segment{
      std::stoi(description[2]), std::stoi(description[3]),
      std::stoi(description[4]), std::stoi(description[5])
    });
}

#undef BUILD_FIELD_DIAG


inline std::unique_ptr<Diagnostic>
Diagnostics_builder::build_diag_particles_energy(
    const std::string& sort_name) {
  return std::make_unique<particles_energy>(
    dir_name + "/", sort_name,
    particles_species_.at(sort_name));
}

inline std::unique_ptr<Diagnostic>
Diagnostics_builder::build_diag_distribution_moment(
    const std::string& sort_name,
    const std::string& moment_name,
    const std::string& axes_names,
    const std::vector<std::string>& description) {
  if (description.size() < 2)
    throw std::runtime_error("Initialization error: No area specified for " + moment_name + "_of_" + axes_names + " diagnostic");

  diag_area area{
    std::stod(description[0]), std::stod(description[1]),
    std::stod(description[2]), std::stod(description[3]),
    std::stod(description[4]), std::stod(description[5]),
  };

  if ((axes_names == "XY" && (
      (0 > area.min[X] || area.min[X] > SIZE_X * dx)   ||
      (0 > area.min[Y] || area.min[Y] > SIZE_Y * dy)   ||
      (0 > area.max[X] || area.max[X] > SIZE_X * dx)   ||
      (0 > area.max[Y] || area.max[Y] > SIZE_Y * dy))) ||
      (axes_names == "VxVy" && (
      (-1.0 > area.min[X] || area.min[X] > 1.0)   ||
      (-1.0 > area.min[Y] || area.min[Y] > 1.0)   ||
      (-1.0 > area.max[X] || area.max[X] > 1.0)   ||
      (-1.0 > area.max[Y] || area.max[Y] > 1.0)))) {
    throw std::runtime_error("Initialization error: Invalid area values for " + moment_name + "_of_" + axes_names + " diagnostic on " + sort_name + " is set");
  }

  return std::make_unique<distribution_moment>(
    dir_name + "/" + sort_name,
    particles_species_.at(sort_name),
    std::make_unique<Moment>(moment_name),
    std::make_unique<Projector2D>(axes_names, area));
}

inline std::unique_ptr<Diagnostic>
Diagnostics_builder::build_diag_x0_distribution_function(
    const std::string& sort_name,
    const std::vector<std::string>& description) {
  int x0 = std::stoi(description[0]);

  diag_area area{
    std::stod(description[1]), std::stod(description[2]),
    std::stod(description[3]), std::stod(description[4]),
    std::stod(description[5]), std::stod(description[6]),
  };

  return std::make_unique<x0_distribution_function>(
    dir_name + "/" + sort_name,
    particles_species_.at(sort_name),
    x0, area);
}
