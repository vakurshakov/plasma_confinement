#include "configuration_storage.hpp"

namespace fs = std::filesystem;

Configuration_storage::Configuration_storage(const std::string& config_path)
  : config_path_(config_path) {
  std::ifstream file(config_path);
  config_ = json::parse(file);
}

void Configuration_storage::init_geometry() const {
  dx = get<double>("Geometry.dx");
  assert(dx > 0 && "Grid step dx is incorrect!");

  SIZE_X = get<int>("Geometry.size_x");
  assert(SIZE_X > 0 && "Grid size along x-axis is incorrect!");

  dy = get<double>("Geometry.dy");
  assert(dy > 0 && "Grid step dy is incorrect!");

  SIZE_Y = get<int>("Geometry.size_y");
  assert(SIZE_Y > 0 && "Grid size along y-axis is incorrect!");

  dt = get<double>("Geometry.dt");
  assert(dt > 0 && "Time step is incorrect!");
  assert(dt * sqrt(1.0 / (dx * dx) + 1.0 / (dy * dy)) <= 1.0 &&
    "2D Courant-Friedrichs-Lewy condition not satisfied!");

  TIME = get<int>("Geometry.time");
  assert(TIME > 0 && "Total number of time steps is incorrect!");

  diagnose_time_step = get<int>("Geometry.diagnose_time_step");
  assert(diagnose_time_step > 0 && "Diagnose time step is incorrect!");

  // Specific asserts for current setup
  assert(dx == dy && "Current setup works only with equal grid step!");
  assert(SIZE_X == SIZE_Y && "Current setup works only with square grid!");
}

void Configuration_storage::save_configuration() const {
  save(config_path_,
    fs::copy_options::overwrite_existing);
}

void Configuration_storage::save_sources() const {
  save("src",
    fs::copy_options::overwrite_existing |
    fs::copy_options::recursive);
}

void Configuration_storage::save(const std::string& from, fs::copy_options options) const {
  try {
    fs::create_directories(get("Out_dir"));
    fs::copy(from, get("Out_dir"), options);
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
