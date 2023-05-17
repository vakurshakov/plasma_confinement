#include "diagnostics_builder.hpp"

#include "src/utils/configuration.hpp"

#include "src/diagnostics/energy.hpp"
#include "src/diagnostics/field_at_point.hpp"
#include "src/diagnostics/field_on_segment.hpp"
#include "src/diagnostics/whole_field.hpp"

#include "src/diagnostics/chosen_particles.hpp"
#include "src/diagnostics/distribution_moment.hpp"
#include "src/diagnostics/x0_distribution_function.hpp"

using std::to_string;

Diagnostics_builder::Diagnostics_builder(
    std::vector<Particles>& particles_species, Fields& fields)
    : fields_(fields), out_dir_(Configuration::out_dir()) {
  for (auto& sort : particles_species) {
    particles_species_.emplace(sort.get_parameters().get_name(), sort);
  }
}

using diagnostic_up = std::unique_ptr<Diagnostic>;
using vector_of_diagnostics = std::vector<diagnostic_up>;

vector_of_diagnostics Diagnostics_builder::build() {
  PROFILE_FUNCTION();
  LOG_TRACE("Building diagnostics...");

  const Configuration& config = Configuration::instance();

  vector_of_diagnostics diagnostics;

  if (!config.contains("Diagnostics")) {
    LOG_WARN("Diagnostics section not found in config file, continuing without them");
    return diagnostics;
  }

  config.for_each("Diagnostics", [&](const Configuration_item& description) {
#if THERE_ARE_FIELDS
    if (description.contains("fields_energy")) {
      diagnostics.emplace_back(build_fields_energy());
    }
    else if (description.contains("field_at_point")) {
      build_field_at_point(description.get_item("field_at_point"), diagnostics);
    }
    else if (description.contains("field_on_segment")) {
      // build_field_on_segment(description, diagnostics);
    }
    else if (description.contains("whole_field")) {
      // build_whole_field(description, diagnostics);
    }
#endif

#if 0
    else if (description.contains("energy")) {
      diagnostics.emplace_back(build_particles_energy(sort));
    }
    else if (description.contains("density")) {
      diagnostics.emplace_back(build_distribution_moment(
        sort, "zeroth_moment", "XY", diag_description));
    }
    else if (description.contains("velocity_distribution")) {
      diagnostics.emplace_back(build_distribution_moment(
        sort, "zeroth_moment", "VxVy", diag_description));
    }
    else if (description.contains("Vx_moment")) {
      diagnostics.emplace_back(build_distribution_moment(
        sort, "Vx_moment", "XY", diag_description));
    }
    else if (description.contains("Vy_moment")) {
      diagnostics.emplace_back(build_distribution_moment(
        sort, "Vy_moment", "XY", diag_description));
    }
    else if (description.contains("Vr_moment")) {
      diagnostics.emplace_back(build_distribution_moment(
        sort, "Vr_moment", "XY", diag_description));
    }
    else if (description.contains("Vphi_moment")) {
      diagnostics.emplace_back(build_distribution_moment(
        sort, "Vphi_moment", "XY", diag_description));
    }
    else if (description.contains("mVxVx_moment")) {
      diagnostics.emplace_back(build_distribution_moment(
        sort, "mVxVx_moment", "XY", diag_description));
    }
    else if (description.contains("mVxVy_moment")) {
      diagnostics.emplace_back(build_distribution_moment(
        sort, "mVxVy_moment", "XY", diag_description));
    }
    else if (description.contains("mVyVy_moment")) {
      diagnostics.emplace_back(build_distribution_moment(
        sort, "mVyVy_moment", "XY", diag_description));
    }
    else if (description.contains("mVrVr_moment")) {
      diagnostics.emplace_back(build_distribution_moment(
        sort, "mVrVr_moment", "XY", diag_description));
    }
    else if (description.contains("mVrVphi_moment")) {
      diagnostics.emplace_back(build_distribution_moment(
        sort, "mVrVphi_moment", "XY", diag_description));
    }
    else if (description.contains("mVphiVphi_moment")) {
      diagnostics.emplace_back(build_distribution_moment(
        sort, "mVphiVphi_moment", "XY", diag_description));
    }
    else if (description.contains("x0_distribution_function")) {
      diagnostics.emplace_back(build_x0_distribution_function(
        sort, diag_description));
    }
#endif
  });

  diagnostics.shrink_to_fit();
  return diagnostics;
}


diagnostic_up Diagnostics_builder::build_fields_energy() {
  return std::make_unique<fields_energy>(
    out_dir_ + "/", fields_.E(), fields_.B());
}

#define TO_CELL(dim, ds) static_cast<int>(round(dim / ds))

void Diagnostics_builder::build_field_at_point(
    const Configuration_item& description,
    vector_of_diagnostics& diagnostics_container) {
  std::list<std::string> field_names = collect_field_names(description);
  std::list<std::string> axis_names = collect_axis_names(description);

  bool check_point = description.contains("point");
  bool check_points = description.contains("points");

  if (!check_point && !check_points) {
    throw std::runtime_error("Initialization error: Point not specified "
      "for point_at_point diagnostic.");
  }
  else if (check_point && check_points) {
    throw std::runtime_error("Initialization error: Both \"point\" and \"points\" "
      "are specified for point_at_point diagnostic.");
  }

  std::list<diag_point> points;
  auto construct_point = [](const Configuration_item& desc) {
    diag_point point;
    point.x = TO_CELL(desc.get<double>("x"), dx);
    point.y = TO_CELL(desc.get<double>("y"), dy);

    if ((0 > point.x || point.x >= SIZE_X) ||
        (0 > point.y || point.y >= SIZE_Y)) {
      throw std::runtime_error("Initialization error: Invalid point for "
        "field_at_point diagnostic is set.");
    }

    return point;
  };

  if (check_point) {
    points.emplace_back(construct_point(description.get_item("point")));
  }
  else {
    description.for_each("points", [&](const Configuration_item& point_desc) {
      points.emplace_back(construct_point(point_desc));
    });
  }

  for (const auto& field_name : field_names) {
  for (const auto& axis_name : axis_names) {
  for (const auto& point : points) {
    LOG_INFO("Add field_on_segment diagnostic for {}", field_name + axis_name);

    const vector3_field& field = get_field(field_name);
    Axis component = get_component(axis_name);

    diagnostics_container.emplace_back(std::make_unique<field_at_point>(
      out_dir_ + "/" + field_name + axis_name + "/",
      "point_(" + to_string(point.x) + "," + to_string(point.y) + ")",
      field, component, point
    ));
  }}}
}

#if 0

void Diagnostics_builder::build_field_on_segment(
    const Configuration_item& description,
    vector_of_diagnostics& diagnostics_container) {
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
    out_dir_ + "/" + description[0] + description[1] + "/segment_(" +
      description[2] + "," + description[3] + ")_(" +
      description[4] + "," + description[5] + ")/",
    get_field(description[0]),
    get_component(description[1]),
    segment);
}

void Diagnostics_builder::build_whole_field(
    const Configuration_item& description,
    vector_of_diagnostics& diagnostics_container) {
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
    out_dir_ + "/" + description[0] + description[1] + "/whole_field/",
    get_field(description[0]),
    get_component(description[1]),
    diag_segment{
      std::stoi(description[2]), std::stoi(description[3]),
      std::stoi(description[4]), std::stoi(description[5])
    });
}
#endif

inline const vector3_field&
Diagnostics_builder::get_field(const std::string& name) {
  if      (name == "E") { return fields_.E(); }
  else if (name == "B") { return fields_.B(); }
  else if (name == "J") { return fields_.J(); }
  else
    throw std::runtime_error("Initialization error: Incorrect field "
      "name in diagnostic description.");
}

std::list<std::string>
Diagnostics_builder::collect_field_names(const Configuration_item& description) {
  bool check_field = description.contains("field");
  bool check_fields = description.contains("fields");

  if (!check_field && !check_fields) {
    throw std::runtime_error("Initialization error: Field not specified "
      "for field_at_point diagnostic.");
  }
  else if (check_field && check_fields) {
    throw std::runtime_error("Initialization error: Both \"field\" and \"fields\" "
      "are specified for field_at_point diagnostic.");
  }

  std::list<std::string> field_names;
  if (check_field) {
    field_names.emplace_back(description.get("field"));
  }
  else {
    description.for_each("fields", [&](const std::string& field_name) {
      field_names.emplace_back(field_name);
    });
  }
  return field_names;
}

inline Axis
Diagnostics_builder::get_component(const std::string& component) {
  if      (component == "x") { return Axis::X; }
  else if (component == "y") { return Axis::Y; }
  else if (component == "z") { return Axis::Z; }
  else
    throw std::runtime_error("Initialization error: Incorrect field "
      "component specified in diagnostic description.");
}

std::list<std::string>
Diagnostics_builder::collect_axis_names(const Configuration_item& description) {
  bool check_component = description.contains("component");
  bool check_components = description.contains("components");

  if (!check_component && !check_components) {
    throw std::runtime_error("Initialization error: Field not specified "
      "for field_at_point diagnostic.");
  }
  else if (check_component && check_components) {
    throw std::runtime_error("Initialization error: Both \"component\" and \"components\" "
      "are specified for field_at_point diagnostic.");
  }

  std::list<std::string> axis_names;
  if (check_component) {
    axis_names.emplace_back(description.get("component"));
  }
  else {
    description.for_each("components", [&](const std::string& comp_name) {
      axis_names.emplace_back(comp_name);
    });
  }
  return axis_names;
}

#undef BUILD_FIELD_DIAG


std::unique_ptr<Diagnostic>
Diagnostics_builder::build_particles_energy(
    const std::string& sort_name) {
  return std::make_unique<particles_energy>(
    out_dir_ + "/", sort_name,
    particles_species_.at(sort_name));
}

std::unique_ptr<Diagnostic>
Diagnostics_builder::build_distribution_moment(
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
    out_dir_ + "/" + sort_name,
    particles_species_.at(sort_name),
    std::make_unique<Moment>(moment_name),
    std::make_unique<Projector2D>(axes_names, area));
}

// std::unique_ptr<Diagnostic>
// Diagnostics_builder::build_x0_distribution_function(
//     const std::string& sort_name,
//     const std::vector<std::string>& description) {
//   int x0 = std::stoi(description[0]);

//   diag_area area{
//     std::stod(description[1]), std::stod(description[2]),
//     std::stod(description[3]), std::stod(description[4]),
//     std::stod(description[5]), std::stod(description[6]),
//   };

//   return std::make_unique<x0_distribution_function>(
//     out_dir_ + "/" + sort_name,
//     particles_species_.at(sort_name),
//     x0, area);
// }

#undef TO_CELL
