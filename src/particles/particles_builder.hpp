#ifndef SRC_PARTICLES_PARTICLES_BUILDER_HPP
#define SRC_PARTICLES_PARTICLES_BUILDER_HPP

#include "src/pch.h"
#include "src/utils/configuration.hpp"

#include "src/fields/fields.hpp"
#include "src/particles/particles.hpp"
#include "src/solvers/abstract_strategies.hpp"

class Particles_builder {
 public:
  Particles_builder(Fields& fields);

  std::vector<Particles> build();

 private:
  Fields& fields_;

  Parameters
  build_parameters(const Configuration_item& description);

  std::unique_ptr<Pusher>
  build_pusher(const Configuration_item& description);

  std::unique_ptr<Interpolation>
  build_interpolation(const Configuration_item& description, const Parameters& params);

  std::unique_ptr<Decomposition>
  build_decomposition(const Configuration_item& description, const Parameters& params);

  /// @todo particles.boundaries_processor_
};

#endif  // SRC_PARTICLES_PARTICLES_BUILDER_HPP
