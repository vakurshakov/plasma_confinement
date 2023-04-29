#ifndef SRC_UTILS_CONFIGURATION_STORAGE_HPP
#define SRC_UTILS_CONFIGURATION_STORAGE_HPP

#include <nlohmann/json.hpp>

#include "src/pch.h"

class Configuration_storage {
 public:
  Configuration_storage(const std::string& path_to_config);

  template<typename T = std::string>
  inline T get(const std::string& key) const;

  void init_geometry() const;
  void save_configuration() const;
  void save_sources() const;

 private:
  using json = nlohmann::json;
  json config_;
  std::string config_path_;

  void save(const std::string& from,
    std::filesystem::copy_options options) const;
};

template<typename T>
inline T Configuration_storage::get(const std::string& key) const {
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

#endif // SRC_UTILS_CONFIGURATION_STORAGE_HPP
