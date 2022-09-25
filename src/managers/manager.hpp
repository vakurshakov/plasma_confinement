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
  Fields fields_;
  std::vector<Particles> particles_species_;

  using Diagnostic_up = std::unique_ptr<Diagnostic>;
  std::vector<Diagnostic_up> diagnostics_;

  void diagnose(int t) const;

  using Command_up = std::unique_ptr<Command>;
  std::list<Command_up> step_presets_;
};