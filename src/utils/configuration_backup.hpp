#ifndef SRC_UTILS_CONFIGURATION_BACKUP_HPP
#define SRC_UTILS_CONFIGURATION_BACKUP_HPP

#include "src/pch.h"

class Configuration_backup {
 public:
  Configuration_backup(std::string to_directory);
  void save_constants();
  void save_sources();

 private:
  std::string to_directory_;

  void save(std::string from,
    std::filesystem::copy_options options);
};

#endif // SRC_UTILS_CONFIGURATION_BACKUP_HPP
