#ifndef SRC_UTILS_CONFIGURATION_HPP
#define SRC_UTILS_CONFIGURATION_HPP

#include <nlohmann/json.hpp>

#include "src/pch.h"

class Configuration_item {
 public:
  template<typename T = std::string>
  T get(const std::string& key) const;

  template<typename T = std::string>
  T get(const std::string& key, T default_value) const;

  bool contains(const std::string& key) const;

  template<typename Function>
  void for_each(const std::string& key, Function func) const;

 protected:
  using json = nlohmann::ordered_json;
  json item_;

  json::json_pointer to_pointer(const std::string& key) const;

  Configuration_item() = default;
  Configuration_item(const json& item);

  Configuration_item(const Configuration_item&) = delete;
  Configuration_item& operator=(const Configuration_item&) = delete;

  Configuration_item(Configuration_item&&) = delete;
  Configuration_item& operator=(Configuration_item&&) = delete;
};


class Configuration : public Configuration_item {
 public:
  static const Configuration& instance(const char* config_path = "");
  static const std::string& out_dir();

  void save() const;
  void save_sources() const;
  void init_geometry() const;

 private:
  std::string out_dir_;
  std::string config_path_;

  void save(const std::string& from,
    std::filesystem::copy_options options) const;

  Configuration(const char* config_path);

  Configuration(const Configuration&) = delete;
  Configuration& operator=(const Configuration&) = delete;

  Configuration(Configuration&&) = delete;
  Configuration& operator=(Configuration&&) = delete;
};


#include "configuration.inl"

#endif // SRC_UTILS_CONFIGURATION_HPP
