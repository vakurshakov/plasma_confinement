#ifndef SRC_PARTICLES_PARTICLES_BUILDER_HPP
#define SRC_PARTICLES_PARTICLES_BUILDER_HPP

#include "src/pch.h"
#include "src/particles/particle/parameters.hpp"
#include "src/solvers/abstract_strategies.hpp"
#include "src/command/command.hpp"
#include "src/fields/fields.hpp"

class Particles_builder {
 public:
  Particles_builder(Fields& fields);

  void set_sort(const std::string& sort_name);

  Parameters build_parameters();

  std::unique_ptr<Pusher>
  build_pusher();
  
  std::unique_ptr<Interpolation>
  build_interpolation(const Parameters&);
  
  std::unique_ptr<Decomposition>
  build_decomposition(const Parameters&);

  std::vector<std::unique_ptr<Particles_diagnostic>>
  build_diagnostics();
 
 private:
  Fields& fields_;

  std::vector<std::string> get_description(const std::string& parameter);

  std::string sort_name_ = "undefined";
  config::umap<string, vector<string>> sort_description_;
  std::vector<std::string> sort_parameters_;
  std::vector<std::string> sort_integration_steps_;
};

#endif  // SRC_PARTICLES_PARTICLES_BUILDER_HPP
