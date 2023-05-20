#include "configuration.hpp"

inline nlohmann::json::json_pointer
Configuration_item::to_pointer(const std::string& key) const {
  std::string copy(key);
  std::replace(copy.begin(), copy.end(), '.', '/');
  return json::json_pointer{"/" + copy};
}

template<typename T>
inline T Configuration_item::get(const std::string& key) const {
  if (key == "") {
    return item_.get<T>();
  }
  return item_.at(to_pointer(key)).get<T>();
}

inline Configuration_item Configuration_item::get_item(const std::string& key) const {
  return Configuration_item{item_.at(to_pointer(key))};
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
  if (item_.is_object()) {
    return item_.contains(to_pointer(key));
  }
  else if (item_.is_string()) {
    return key == item_.get<std::string>();
  }

  return false;
}

inline bool Configuration_item::is_array(const std::string& key) const {
  if (key == "") {
    return item_.is_array();
  }
  return item_.at(to_pointer(key)).is_array();
}

inline void Configuration_item::for_each(const std::string& key, item_parser func) const {
  for (const auto& element : item_.at(to_pointer(key))) {
    func(Configuration_item{element});
  }
}

inline void Configuration_item::for_each(item_parser func) const {
  for (const auto& element : item_) {
    func(Configuration_item{element});
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
