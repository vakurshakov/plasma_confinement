#include "src/managers/manager.hpp"
#include "src/utils/configuration_backup.hpp"

int main(int argc, char const *argv[]) {
  LOG_INIT(dir_name + "/simulation.log" );
  BEGIN_SESSION(dir_name + "/simulation_time_profile.json");

  {
    Configuration_backup config(dir_name);
    config.save_constants();
  }

  Manager manager;
  manager.initializes();
  manager.calculates();

  END_SESSION();
  return EXIT_SUCCESS;
}
