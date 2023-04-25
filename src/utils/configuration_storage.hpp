#ifndef SRC_UTILS_CONFIGURATION_STORAGE_HPP
#define SRC_UTILS_CONFIGURATION_STORAGE_HPP

#include <nlohmann/json.hpp>

#include "src/pch.h"

class Configuration_storage {
 public:
  Configuration_storage(const std::string& path_to_config);

  template<typename T = double>
  constexpr T get(const std::string& key);

  void save_constants();
  void save_sources();

 private:
  using json = nlohmann::json;
  json config_;
  std::string config_path_;

  void save(const std::string& from,
    std::filesystem::copy_options options);
};

template<typename T>
constexpr T Configuration_storage::get(const std::string& key) {
  return config_.at(key).get<T>();
}

#endif // SRC_UTILS_CONFIGURATION_STORAGE_HPP
