#include "configuration_storage.hpp"

namespace fs = std::filesystem;

Configuration_storage::Configuration_storage(const std::string& config_path)
  : config_path_(config_path) {
  std::ifstream file(config_path);
  config_ = json::parse(file);
}

void Configuration_storage::save_constants() {
  save(config_path_,
    fs::copy_options::overwrite_existing);
}

void Configuration_storage::save_sources() {
  save("src",
    fs::copy_options::overwrite_existing |
    fs::copy_options::recursive);
}

void Configuration_storage::save(const std::string& from, fs::copy_options options) {
  try {
    fs::create_directories(config_.at("Out_dir"));
    fs::copy(from, config_.at("Out_dir"), options);
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
