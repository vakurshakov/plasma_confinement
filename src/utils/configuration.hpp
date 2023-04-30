#ifndef SRC_UTILS_CONFIGURATION_HPP
#define SRC_UTILS_CONFIGURATION_HPP

#include <nlohmann/json.hpp>

#include "src/pch.h"

class Configuration {
 public:
  static Configuration& instance(const char* config_path = "");
  static const std::string& out_dir();

  template<typename T = std::string>
  inline T get(const std::string& key) const;

  void save() const;
  void save_sources() const;
  void init_geometry() const;

 private:
  using json = nlohmann::json;
  json config_;
  std::string config_path_;

  std::string out_dir_;

  void save(const std::string& from,
    std::filesystem::copy_options options) const;

  Configuration(const char* config_path);

  Configuration(const Configuration&) = delete;
  Configuration& operator=(const Configuration&) = delete;

  Configuration(Configuration&&) = delete;
  Configuration& operator=(Configuration&&) = delete;
};


/* static */ inline Configuration& Configuration::instance(const char* config_path) {
  static Configuration instance(config_path);
  return instance;
}

/* static */ inline const std::string& Configuration::out_dir() {
  return instance().out_dir_;
}

template<typename T>
inline T Configuration::get(const std::string& key) const {
  json result = config_;

  size_t start = 0;
  size_t end = -1;
  do {
    end = key.find(".", start);

    result = result.at(key.substr(start, end - start));
    start = end + 1;
  }
  while (end != std::string::npos);

  return result.get<T>();
}

#endif // SRC_UTILS_CONFIGURATION_HPP
