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
      build_field_on_segment(description.get_item("field_on_segment"), diagnostics);
    }
    else if (description.contains("whole_field")) {
      build_whole_field(description.get_item("whole_field"), diagnostics);
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
  std::list<Field_description> field_descriptions = collect_field_descriptions(description);

  bool check_point = description.contains("point");
  bool check_points = description.contains("points");

  if (!check_point && !check_points) {
    throw std::runtime_error("Initialization error: Point not specified "
      "for field_at_point diagnostic.");
  }
  else if (check_point && check_points) {
    throw std::runtime_error("Initialization error: Both \"point\" and \"points\" "
      "are specified for field_at_point diagnostic.");
  }

  auto construct_point = [&](const Configuration_item& desc) {
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

  std::list<diag_point> points;
  if (check_point) {
    points.emplace_back(construct_point(description.get_item("point")));
  }
  else {
    description.for_each("points", [&](const Configuration_item& point_desc) {
      points.emplace_back(construct_point(point_desc));
    });
  }

  for (const auto& point : points) {
  for (const auto& field_description : field_descriptions) {
  for (const auto& [comp_name, comp_axis] : field_description.components) {
    LOG_INFO("Add field_on_segment diagnostic for {}", field_description.field.first + comp_name);

    diagnostics_container.emplace_back(std::make_unique<field_at_point>(
      out_dir_ + "/" + field_description.field.first + comp_name + "/",
      "point_(" + to_string(point.x) + "," + to_string(point.y) + ")",
      *field_description.field.second, comp_axis, point
    ));
  }}}
}

void Diagnostics_builder::build_field_on_segment(
    const Configuration_item& description,
    vector_of_diagnostics& diagnostics_container) {
    std::list<Field_description> field_descriptions = collect_field_descriptions(description);

  bool check_segment = description.contains("segment");
  bool check_segments = description.contains("segments");

  if (!check_segment && !check_segments) {
    throw std::runtime_error("Initialization error: Segment not specified "
      "for field_on_segment diagnostic.");
  }
  else if (check_segment && check_segments) {
    throw std::runtime_error("Initialization error: Both \"segment\" and \"segments\" "
      "are specified for field_on_segment diagnostic.");
  }

  auto construct_segment = [&](const Configuration_item& desc) {
    diag_segment segment;
    segment.begin[X] = TO_CELL(desc.get<double>("begin.x"), dx);
    segment.begin[Y] = TO_CELL(desc.get<double>("begin.y"), dx);
    segment.end[X] = TO_CELL(desc.get<double>("end.x"), dx);
    segment.end[Y] = TO_CELL(desc.get<double>("end.y"), dy);

    if ((segment.begin[X] < 0 || segment.begin[X] > SIZE_X) ||
        (segment.begin[Y] < 0 || segment.begin[Y] > SIZE_Y) ||
        (segment.end[X] < 0 || segment.end[X] > SIZE_X) ||
        (segment.end[Y] < 0 || segment.end[Y] > SIZE_Y) ||
        (segment.end[X] < segment.begin[X]) ||
        (segment.end[Y] < segment.begin[Y])) {
      throw std::runtime_error("Initialization error: Invalid segment "
        "for field_on_segment diagnostic is set.");
    }
    return segment;
  };

  std::list<diag_segment> segments;
  if (check_segment) {
    segments.emplace_back(construct_segment(description.get_item("segment")));
  }
  else {
    description.for_each("segments", [&](const Configuration_item& segment_desc) {
      segments.emplace_back(construct_segment(segment_desc));
    });
  }

  for (const auto& segment : segments) {
  for (const auto& field_description : field_descriptions) {
  for (const auto& [comp_name, comp_axis] : field_description.components) {
    LOG_INFO("Add field_on_segment diagnostic for {}", field_description.field.first + comp_name);

    diagnostics_container.emplace_back(std::make_unique<field_on_segment>(
      out_dir_ + "/" + field_description.field.first + comp_name + "/segment_(" +
      to_string(segment.begin[X]) + "," + to_string(segment.begin[Y]) + ")_(" +
      to_string(segment.end[X]) + "," + to_string(segment.end[Y]) + ")/",
      *field_description.field.second, comp_axis, segment));
  }}}
}

void Diagnostics_builder::build_whole_field(
    const Configuration_item& description,
    vector_of_diagnostics& diagnostics_container) {
  std::list<Field_description> field_descriptions = collect_field_descriptions(description);

  auto construct_area = [&](const Configuration_item& desc) {
    diag_segment area;
    area.begin[X] = TO_CELL(desc.get<double>("begin.x"), dx);
    area.begin[Y] = TO_CELL(desc.get<double>("begin.y"), dx);
    area.end[X] = TO_CELL(desc.get<double>("end.x"), dx);
    area.end[Y] = TO_CELL(desc.get<double>("end.y"), dy);

    if ((area.begin[X] < 0 || area.begin[X] > SIZE_X) ||
        (area.begin[Y] < 0 || area.begin[Y] > SIZE_Y) ||
        (area.end[X] < 0 || area.end[X] > SIZE_X) ||
        (area.end[Y] < 0 || area.end[Y] > SIZE_Y) ||
        (area.end[X] <= area.begin[X]) ||
        (area.end[Y] <= area.begin[Y])) {
      throw std::runtime_error("Initialization error: Invalid area "
        "for whole_field diagnostic is set.");
    }
    return area;
  };

  diag_segment area;
  if (description.contains("area")) {
    area = construct_area(description.get_item("area"));
  }
  else {
    area.begin[X] = 0;
    area.begin[Y] = 0;
    area.end[X] = SIZE_X;
    area.end[Y] = SIZE_Y;
  }

  for (const auto& field_description : field_descriptions) {
  for (const auto& [comp_name, comp_axis] : field_description.components) {
    LOG_INFO("Add whole_field diagnostic for {}", field_description.field.first + comp_name);

    diagnostics_container.emplace_back(std::make_unique<whole_field>(
      out_dir_ + "/" + field_description.field.first + comp_name + "/whole_field/",
      *field_description.field.second, comp_axis, area
    ));
  }}
}

Diagnostics_builder::Field_description
Diagnostics_builder::create_field_description(const Configuration_item& field_desc) {
  auto get_component = [&](const Configuration_item& component_desc) {
    if      (component_desc.contains("x")) { return std::make_pair("x", Axis::X); }
    else if (component_desc.contains("y")) { return std::make_pair("y", Axis::Y); }
    else if (component_desc.contains("z")) { return std::make_pair("z", Axis::Z); }
    else {
      throw std::runtime_error("Initialization error: Incorrect field "
        "component specified in diagnostic description.");
    }
  };

  Field_description result;
  if      (field_desc.contains("E")) { result.field = std::make_pair("E", &fields_.E()); }
  else if (field_desc.contains("B")) { result.field = std::make_pair("B", &fields_.B()); }
  else if (field_desc.contains("J")) { result.field = std::make_pair("J", &fields_.J()); }
  else {
    throw std::runtime_error("Initialization error: Incorrect field "
      "name in diagnostic description.");
  }

  if (Configuration_item item = field_desc.get_item(result.field.first); !item.is_array()) {
    result.components.emplace(get_component(item));
  }
  else {
    item.for_each([&](const Configuration_item& subitem) {
      const auto& [_, success] = result.components.emplace(get_component(subitem));
      if (!success) {
        throw std::runtime_error("Initialization error: Component was used twice "
          "in diagnostic description for " + result.field.first + " field.");
      }
    });
  }
  return result;
}

std::list<Diagnostics_builder::Field_description>
Diagnostics_builder::collect_field_descriptions(const Configuration_item& description) {
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

  std::set<std::string> field_names;
  std::list<Field_description> field_descriptions;
  if (check_field) {
    field_descriptions.emplace_back(create_field_description(description.get_item("field")));
  }
  else {
    description.for_each("fields", [&](const Configuration_item& field_description) {
      const auto& desc = field_descriptions.emplace_back(create_field_description(field_description));
      const auto& [_, success] = field_names.emplace(desc.field.first);
      if (!success) {
        throw std::runtime_error("Initialization error: Field name was "
          "used twice in \"fields\" for diagnostic description.");
      }
    });
  }
  return field_descriptions;
}


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
