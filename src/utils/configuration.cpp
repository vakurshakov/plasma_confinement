#include "configuration.hpp"

namespace fs = std::filesystem;

Configuration_item::Configuration_item(const json& json) {
  item_ = json;
}


Configuration::Configuration(const char* config_path)
    : config_path_(config_path) {
  std::ifstream file(config_path);
  item_ = json::parse(file);
  out_dir_ = get("Out_dir");
}

void Configuration::init_geometry() const {
  dx = get<double>("Geometry.dx");
  assert(dx > 0 && "Grid step dx is incorrect!");

  dy = get<double>("Geometry.dy");
  assert(dy > 0 && "Grid step dy is incorrect!");

  dt = get<double>("Geometry.dt");
  assert(dt > 0 && "Time step is incorrect!");
  assert(dt * sqrt(1.0 / (dx * dx) + 1.0 / (dy * dy)) <= 1.0 &&
    "2D Courant-Friedrichs-Lewy condition not satisfied!");

#define TO_STEP(dim, ds) static_cast<int>(floor(dim / ds))

  SIZE_X = TO_STEP(get<double>("Geometry.size_x"), dx);
  assert(SIZE_X > 0 && "Grid size along x-axis is incorrect!");

  SIZE_Y = TO_STEP(get<double>("Geometry.size_y"), dy);
  assert(SIZE_Y > 0 && "Grid size along y-axis is incorrect!");

  TIME = TO_STEP(get<double>("Geometry.time"), dt);
  assert(TIME > 0 && "Total number of time steps is incorrect!");

  diagnose_time_step = TO_STEP(get<double>("Geometry.diagnose_time_step"), dt);
  assert(diagnose_time_step > 0 && "Diagnose time step is incorrect!");

#undef TO_STEP
}

void Configuration::save(const std::string& to) const {
  save(config_path_, to,
    fs::copy_options::overwrite_existing);
}

void Configuration::save_sources(const std::string& to) const {
  save("src/", to,
    fs::copy_options::overwrite_existing |
    fs::copy_options::recursive);
}

void Configuration::save(const std::string& from, const std::string& to,
    fs::copy_options options) const {
  try {
    fs::create_directories(out_dir() + "/" + to + "/");
    fs::copy(from, out_dir() + "/" + to + "/", options);
  }
  catch(const fs::filesystem_error& ex) {
    std::stringstream ss;

    ss << "what():  " << ex.what() << '\n'
       << "path1(): " << ex.path1() << '\n'
       << "path2(): " << ex.path2() << '\n'
       << "code().value():    " << ex.code().value() << '\n'
       << "code().message():  " << ex.code().message() << '\n'
       << "code().category(): " << ex.code().category().name() << '\n';

    throw std::runtime_error(ss.str());
  }
}
