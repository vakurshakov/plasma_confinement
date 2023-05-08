#include "configuration.hpp"

inline nlohmann::json::json_pointer
Configuration_item::to_pointer(const std::string& key) const {
  std::string copy(key);
  std::replace(copy.begin(), copy.end(), '.', '/');
  return json::json_pointer("/" + copy);
}

template<typename T>
inline T Configuration_item::get(const std::string& key) const {
  return item_.at(to_pointer(key)).get<T>();
}

template<typename T>
inline T Configuration_item::get(const std::string& key, T default_value) const {
  auto pointer = to_pointer(key);
  if (item_.contains(pointer)) {
    return item_.at(pointer).get<T>();
  }
  return default_value;
}

inline bool Configuration_item::contains(const std::string& key) const {
  return item_.contains(to_pointer(key));
}

template<typename Function>
inline void Configuration_item::for_each(const std::string& key, Function func) const {
  for (const auto& element : item_.at(to_pointer(key))) {
    func(Configuration_item(element));
  }
}

/* static */ inline
const Configuration& Configuration::instance(const char* config_path) {
  static Configuration instance(config_path);
  return instance;
}

/* static */ inline
const std::string& Configuration::out_dir() {
  return instance().out_dir_;
}