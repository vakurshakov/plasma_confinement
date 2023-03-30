#ifndef SRC_MANAGERS_MANAGER_HPP
#define SRC_MANAGERS_MANAGER_HPP

#include "src/pch.h"
#include "src/fields/fields.hpp"
#include "src/particles/particles.hpp"
#include "src/diagnostics/diagnostic.hpp"
#include "src/command/command.hpp"

class Manager {
 public:
  Manager() = default;

  void initializes();
  void calculates();

 private:
  void log_information() const;

  size_t START_ = 0u;

  Fields fields_;
  std::vector<Particles> particles_species_;

  using Diagnostic_up = std::unique_ptr<Diagnostic>;
  std::vector<Diagnostic_up> diagnostics_;

  void diagnose(size_t t) const;

  using Command_up = std::unique_ptr<Command>;
  std::list<Command_up> step_presets_;
};

#endif  // SRC_MANAGERS_MANAGER_HPP
