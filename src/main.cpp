#include <iostream>

#include "src/constants.h"
#include "src/utils/configuration_storage.hpp"
// #include "src/managers/manager.hpp"

int main(int argc, const char** argv) {
  if (argc == 1) {
    std::cerr << "usage: simulation.out <config.json>\n" << std::endl;
    return EXIT_FAILURE;
  }

  Configuration_storage config(argv[1]);
  config.save_constants();

  LOG_INIT(config.get<std::string>("Out_dir") + "/simulation.log");
  BEGIN_SESSION(config.get<std::string>("Out_dir") + "/simulation_time_profile.json");

  // Manager manager;
  // manager.initializes();
  // manager.calculates();

  END_SESSION();
  return EXIT_SUCCESS;
}
