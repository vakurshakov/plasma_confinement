#include "src/managers/configuration_saver.hpp"
#include "src/managers/manager.hpp"

int main(int argc, char const *argv[]) {
  LOG_INIT(dir_name + "/simulation.log" );
  BEGIN_SESSION(dir_name + "/simulation_time_profile.json");

  {
    Configuration_saver configuration_saver;
    configuration_saver.save();
  }

  Manager manager;
  manager.initializes();
  manager.calculates();

  END_SESSION();
  return EXIT_SUCCESS;
}
