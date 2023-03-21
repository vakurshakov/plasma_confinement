#include "configuration_backup.hpp"

namespace fs = std::filesystem;

Configuration_backup::Configuration_backup(std::string to_directory)
    : to_directory_(to_directory) {}

void Configuration_backup::save_constants() {
  save("src/constants.h",
    fs::copy_options::overwrite_existing);
}

void Configuration_backup::save_sources() {
  save("src",
    fs::copy_options::overwrite_existing |
    fs::copy_options::recursive);
}

void Configuration_backup::save(std::string from, fs::copy_options options) {
  try {
    fs::copy(from, to_directory_, options);
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


