#include "src/simulation/simulation.hpp"

void Simulation::initialize(int argc, const char* argv[]) {
  this->initialize_implementation(argc, argv);

  #pragma omp parallel for shared(diagnostics)
  for(const auto& diagnostic : diagnostics) {
    diagnostic->diagnose(0);
  }
}


void Simulation::run() {
  for (time_step t = 0; t <= TIME; ++t) {
    this->step_implementation(t);

    #pragma omp parallel for shared(diagnostics)
    for(const auto& diagnostic : diagnostics) {
      diagnostic->diagnose(t);
    }
  }
}