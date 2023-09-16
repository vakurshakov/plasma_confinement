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

/// @todo Remove boilerplate code in diagnostic build up
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

#if THERE_ARE_PARTICLES
    else if (description.contains("energy")) {
      build_particles_energy(description.get_item("particles_energy"), diagnostics);
    }
    else if (description.contains("distribution_moment")) {
      build_distribution_moment(description.get_item("distribution_moment"), diagnostics);
    }
#if 0
    else if (description.contains("x0_distribution_function")) {
      diagnostics.emplace_back(build_x0_distribution_function(
        sort, diag_description));
    }
#endif
#endif
  });

  /// @todo check on uniqueness using diagnostic.result_directory_
  diagnostics.shrink_to_fit();
  return diagnostics;
}


diagnostic_up Diagnostics_builder::build_fields_energy() {
  return std::make_unique<fields_energy>(
    out_dir_ + "/", fields_.E(), fields_.B());
}

#define TO_CELL(dim, ds) static_cast<int>(round(dim / ds))

bool check_consistency(const Configuration_item& description,
    const std::string& description_name, const std::string& __diag_name) {
  bool check_singular = description.contains(description_name);
  bool check_plural = description.contains(description_name + "s");

  if (!check_singular && !check_plural) {
    throw std::runtime_error("Initialization error: \"" + description_name + "\" "
      "not specified for " + __diag_name + " diagnostic.");
  }
  else if (check_singular && check_plural) {
    throw std::runtime_error("Initialization error: Both \"" + description_name + "\" "
      "and \"" + description_name + "s\" are specified for " + __diag_name + " diagnostic.");
  }
  return check_singular;
}

void Diagnostics_builder::build_field_at_point(
    const Configuration_item& description,
    vector_of_diagnostics& diagnostics_container) {
  std::list<Field_description> field_descriptions =
    collect_field_descriptions(description, "field_at_point");

  bool check_point = check_consistency(description, "point", "field_at_point");

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
    description.for_each("points", [&](const Configuration_item& point_description) {
      points.emplace_back(construct_point(point_description));
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

diag_segment construct_segment(const Configuration_item& description,
    bool is_touching_allowed, const std::string& __diag_name) {
  diag_segment segment;
  segment.begin[X] = TO_CELL(description.get<double>("begin.x"), dx);
  segment.begin[Y] = TO_CELL(description.get<double>("begin.y"), dx);
  segment.end[X] = TO_CELL(description.get<double>("end.x"), dx);
  segment.end[Y] = TO_CELL(description.get<double>("end.y"), dy);

  bool is_begin_end_reversed = !is_touching_allowed ?
    (segment.end[X] <= segment.begin[X]) || (segment.end[Y] <= segment.begin[Y]) :
    (segment.end[X] < segment.begin[X]) || (segment.end[Y] < segment.begin[Y]);

  if ((segment.begin[X] < 0 || segment.begin[X] > SIZE_X) ||
      (segment.begin[Y] < 0 || segment.begin[Y] > SIZE_Y) ||
      (segment.end[X] < 0 || segment.end[X] > SIZE_X) ||
      (segment.end[Y] < 0 || segment.end[Y] > SIZE_Y) ||
      is_begin_end_reversed) {
    throw std::runtime_error("Initialization error: Invalid region "
      "for " + __diag_name + " diagnostic is set.");
  }
  return segment;
}

void Diagnostics_builder::build_field_on_segment(
    const Configuration_item& description,
    vector_of_diagnostics& diagnostics_container) {
  std::list<Field_description> field_descriptions =
    collect_field_descriptions(description, "field_on_segment");

  bool check_segment = check_consistency(description, "segment", "field_on_segment");

  std::list<diag_segment> segments;
  if (check_segment) {
    segments.emplace_back(construct_segment(description.get_item("segment"),
      /* is_touching_allowed */ true, "field_on_segment"));
  }
  else {
    description.for_each("segments", [&](const Configuration_item& segment_description) {
      segments.emplace_back(construct_segment(segment_description,
        /* is_touching_allowed */ true, "field_on_segment"));
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
  std::list<Field_description> field_descriptions =
    collect_field_descriptions(description, "whole_field");

  diag_segment area;
  if (description.contains("area")) {
    area = construct_segment(description.get_item("area"),
      /* is_touching_allowed */ false, "whole_field");
  }
  else {
    LOG_WARN("Area for whole_field diagnostic not specified, using");
    LOG_WARN("  the default one: begin = {0, 0}, end = {Lx, Ly}.");

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

std::list<Diagnostics_builder::Field_description>
Diagnostics_builder::collect_field_descriptions(
    const Configuration_item& description, const std::string& __diag_name) {
  bool check_field = check_consistency(description, "field", __diag_name);

  std::set<std::string> field_names;
  std::list<Field_description> field_descriptions;
  if (check_field) {
    field_descriptions.emplace_back(create_field_description(description.get_item("field")));
  }
  else {
    description.for_each("fields", [&](const Configuration_item& field_description) {
      const auto& desc = field_descriptions.emplace_back(create_field_description(field_description));
      if (const auto& [_, success] = field_names.emplace(desc.field.first); !success) {
        throw std::runtime_error("Initialization error: Field name was "
          "used twice in \"fields\" for " + __diag_name + " diagnostic.");
      }
    });
  }
  return field_descriptions;
}

Diagnostics_builder::Field_description
Diagnostics_builder::create_field_description(const Configuration_item& field_desc) {
  Field_description result;
  if      (field_desc.contains("E")) { result.field = std::make_pair("E", &fields_.E()); }
  else if (field_desc.contains("B")) { result.field = std::make_pair("B", &fields_.B()); }
  else if (field_desc.contains("J")) { result.field = std::make_pair("J", &fields_.J()); }
  else {
    throw std::runtime_error("Initialization error: Incorrect field "
      "name in diagnostic description.");
  }

  auto get_component = [&](const Configuration_item& component_desc) {
    if      (component_desc.contains("x")) { return std::make_pair("x", Axis::X); }
    else if (component_desc.contains("y")) { return std::make_pair("y", Axis::Y); }
    else if (component_desc.contains("z")) { return std::make_pair("z", Axis::Z); }
    else {
      throw std::runtime_error("Initialization error: Incorrect field "
        "component specified in diagnostic description.");
    }
  };

  if (Configuration_item item = field_desc.get_item(result.field.first); !item.is_array()) {
    result.components.emplace(get_component(item));
  }
  else {
    item.for_each([&](const Configuration_item& subitem) {
      if (const auto& [_, success] = result.components.emplace(get_component(subitem)); !success) {
        throw std::runtime_error("Initialization error: Component was used twice "
          "in diagnostic description for " + result.field.first + " field.");
      }
    });
  }
  return result;
}


std::set<std::string> collect_string_descriptions(const Configuration_item& description,
    const std::string& description_name, const std::string& __diag_name) {
  bool check_name = check_consistency(description, description_name, __diag_name);

  std::set<std::string> particles_names;
  if (check_name) {
    particles_names.emplace(description.get(description_name));
  }
  else {
    description.for_each(description_name + "s", [&](const Configuration_item& field_description){
      if (const auto& [_, success] = particles_names.emplace(field_description.get()); !success) {
        throw std::runtime_error("Initialization error: Particles \"name\" was "
          "used twice in " + __diag_name + " diagnostic.");
      }
    });
  }
  return particles_names;
}

void Diagnostics_builder::build_particles_energy(
    const Configuration_item& description,
    vector_of_diagnostics& diagnostics_container) {
  std::set<std::string> particles_names =
    collect_string_descriptions(description, "particles_name", "particles_energy");

  for (const auto& particles_name : particles_names) {
    LOG_INFO("Add particles_energy diagnostic for {}", particles_name);

    diagnostics_container.emplace_back(std::make_unique<particles_energy>(
      out_dir_ + "/", particles_name, particles_species_.at(particles_name)));
  }
}

diag_area build_diag_area(const Configuration_item& description,
    const std::string& axes_name) {
  diag_area area;
  area.min[X] = description.get<double>("min.x");
  area.min[Y] = description.get<double>("min.y");
  area.max[X] = description.get<double>("max.x");
  area.max[Y] = description.get<double>("max.y");
  area.dp[X]  = description.get<double>("dp.x");
  area.dp[Y]  = description.get<double>("dp.y");

  if ((axes_name == "XY" && (
      (area.min[X] < 0.0 || area.min[X] > SIZE_X * dx) ||
      (area.min[Y] < 0.0 || area.min[Y] > SIZE_Y * dy) ||
      (area.max[X] < 0.0 || area.max[X] > SIZE_X * dx) ||
      (area.max[Y] < 0.0 || area.max[Y] > SIZE_Y * dy))) ||
      (axes_name == "VxVy" && (
      (area.min[X] < -1.0 || area.min[X] > 1.0) ||
      (area.min[Y] < -1.0 || area.min[Y] > 1.0) ||
      (area.max[X] < -1.0 || area.max[X] > 1.0) ||
      (area.max[Y] < -1.0 || area.max[Y] > 1.0))) ||
      ((area.dp[X] <= 0) || area.dp[Y] <= 0) ||
      ((area.max[X] < area.min[X]) || (area.max[Y] < area.min[Y]))) {
    throw std::runtime_error("Initialization error: Invalid \"" + axes_name +
      "\" area for distribution_moment diagnostic is set.");
  }
  return area;
}

void Diagnostics_builder::build_distribution_moment(
    const Configuration_item& description,
    vector_of_diagnostics& diagnostics_container) {
  std::set<std::string> particles_names =
    collect_string_descriptions(description, "particles_name", "distribution_moment");

  std::set<std::string> __moment_names =
    collect_string_descriptions(description, "moment", "distribution_moment");

  bool has_XY = false;
  bool has_VxVy = false;
  std::list<std::pair<std::string, std::string>> moment_names;
  for (const auto& moment_name : __moment_names) {
    std::string axes_name;
    if (moment_name == "density" ||
        moment_name == "Vx_moment" ||
        moment_name == "Vy_moment" ||
        moment_name == "Vr_moment" ||
        moment_name == "Vphi_moment" ||
        moment_name == "mVxVx_moment" ||
        moment_name == "mVxVy_moment" ||
        moment_name == "mVyVy_moment" ||
        moment_name == "mVrVr_moment" ||
        moment_name == "mVrVphi_moment" ||
        moment_name == "mVphiVphi_moment") {
      axes_name = "XY";
      has_XY = true;
    }
    else if (moment_name == "velocity_distribution") {
      axes_name = "VxVy";
      has_VxVy = true;
    }
    else {
      throw std::runtime_error("Initialization error: Unknown moment name"
        "for distribution_moment diagnostic.");
    }

    auto& back = moment_names.emplace_back(std::make_pair(moment_name, axes_name));
    if (back.first == "density" || back.first == "velocity_distribution") {
      back.first = "zeroth_moment";
    }
  }

  bool check_area = check_consistency(description, "area", "distribution_moment");
  diag_area XY_area {
    .min = {0.0, 0.0},
    .max = {SIZE_X * dx, SIZE_Y * dy},
    .dp  = {dx, dy},
  };

  diag_area VxVy_area {
    .min = {-1.0, -1.0},
    .max = {+1.0, +1.0},
    .dp  = {0.01, 0.01},
  };

  std::string area = check_area ? "area" : "areas";
  bool is_XY = false;
  bool is_VxVy = false;

  if (is_XY = description.contains(area + ".XY"); is_XY && has_XY) {
    XY_area = build_diag_area(description.get_item(area + ".XY"), "XY");
  }

  if (is_VxVy = description.contains(area + ".VxVy"); is_VxVy && has_VxVy) {
    VxVy_area = build_diag_area(description.get_item(area + ".VxVy"), "VxVy");
  }

  if (is_XY && has_XY && has_VxVy) {
    LOG_WARN("Diagnostic of distribution_moment without an \"" + area + ".VxVy\" found,");
    LOG_WARN("  using the default one: min = {-1, -1}, max = {+1, +1}, dp = {1e-2, 1e-2}");
  }
  else if (is_VxVy && has_XY && has_VxVy) {
    LOG_WARN("Diagnostic of distribution_moment without an \"" + area + ".XY\" found,");
    LOG_WARN("  using the default one: min = {0, 0}, max = {Lx, Ly}, dp = {dx, dy}");
  }
  else {
    throw std::runtime_error("Initialization error: Incorrect use of diagnostic"
      " \"" + area + "\" for distribution_moment diagnostic.");
  }

  for (const auto& particles_name : particles_names) {
  for (const auto& [moment_name, axes_name] : moment_names) {
    LOG_INFO("Add {}_of_{} diagnostic for {}", moment_name, axes_name, particles_name);

    std::make_unique<distribution_moment>(
      out_dir_ + "/" + particles_name,
      particles_species_.at(particles_name),
      std::make_unique<Moment>(moment_name),
      std::make_unique<Projector2D>(axes_name,
        (axes_name == "XY") ? XY_area : VxVy_area));
  }}
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
