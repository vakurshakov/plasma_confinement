#include "src/managers/configuration_saver.hpp"
#include "src/managers/manager.hpp"

int main(int argc, char const *argv[]) {
  /// @todo Date and time of the log, to separate them automatically
  LOG_INIT("log/simulation.log");
  BEGIN_SESSION("log/simulation_time_profile.json");

  {
    Configuration_saver configuration_saver;
    configuration_saver.save();
  }

  Manager Nerpa;
  Nerpa.initializes();
  Nerpa.calculates();

  END_SESSION();
  return EXIT_SUCCESS;
}