#ifndef SRC_SIMULATION_SIMULATION_HPP
#define SRC_SIMULATION_SIMULATION_HPP

#include "src/pch.h"
#include "src/diagnostics/diagnostic.hpp"

class Simulation {
public:
  Simulation() = default;

	Simulation(const Simulation&) = delete;
  Simulation& operator=(const Simulation&) = delete;

  virtual ~Simulation();

  virtual void show_description() const = 0;

  // Template method: it only relies on implementation
  // and makes an initial diagnostics at the end.
  void initialize(int argc, const char* argv[]);
  
  // Template method: runs the main time loop, internally
  // uses the `step implementation` and at the end of each
  // step, runs each diagnostic from the list.
  void run();

 protected:
  virtual void initialize_implementation(int argc, const char* argv[]) = 0;
  virtual void step_implementation(time_step t) = 0;

  using Diagnostic_up = std::unique_ptr<Diagnostic>;
  std::vector<Diagnostic_up> diagnostics;
};

#endif  // SRC_SIMULATION_SIMULATION_HPP
