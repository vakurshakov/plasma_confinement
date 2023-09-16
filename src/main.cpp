#include <iostream>

#include "src/pch.h"
#include "src/managers/manager.hpp"
#include "src/utils/configuration.hpp"

int main(int argc, const char** argv) {
  if (argc == 1) {
    std::cerr << "usage: simulation.out <config.json>\n" << std::endl;
    return EXIT_FAILURE;
  }

  /// @warning An exception may be thrown during _initialization_,
  /// if the problem is misconfigured using <config.json>
  try {
    const Configuration& config = Configuration::instance(argv[1]);
    config.save();
    config.init_geometry();

    LOG_INIT(config.out_dir() + "/simulation.log");
    BEGIN_SESSION(config.out_dir() + "/simulation_time_profile.json");

    Manager manager;
    manager.initializes();
    manager.calculates();
  }
  catch (const std::exception& e) {
    std::cerr << " what(): " << e.what() << std::endl;
  }

  END_SESSION();
  return EXIT_SUCCESS;
}
