#ifndef SRC_PARTICLES_PARTICLES_BUILDER_HPP
#define SRC_PARTICLES_PARTICLES_BUILDER_HPP

#include "src/pch.h"
#include "src/utils/configuration.hpp"

#include "src/fields/fields.hpp"
#include "src/particles/particles.hpp"
#include "src/solvers/abstract_strategies.hpp"
#include "src/particles/particles_boundary_interaction.hpp"

class Particles_builder {
 public:
  Particles_builder(Fields& fields);

  std::vector<Particles> build();

 private:
  Fields& fields_;
  std::map<std::string, Particles&> named_species_;

  Parameters
  build_parameters(const Configuration_item& description);

  std::unique_ptr<Pusher>
  build_pusher(const Configuration_item& description);

  std::unique_ptr<Interpolation>
  build_interpolation(const Configuration_item& description,
    const std::string& sort_name);

  std::unique_ptr<Decomposition>
  build_decomposition(const Configuration_item& description,
    const std::string& sort_name);

  std::unique_ptr<Particles_boundary>
  build_particles_boundary(const Configuration_item& description,
    const std::string& sort_name);
};

#endif  // SRC_PARTICLES_PARTICLES_BUILDER_HPP
