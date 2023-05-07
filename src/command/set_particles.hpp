#ifndef SRC_COMMAND_SET_PARTICLES_HPP
#define SRC_COMMAND_SET_PARTICLES_HPP

#include "command.hpp"

#include "src/pch.h"
#include "src/particles/particles.hpp"
#include "src/utils/domain_geometry.hpp"

/**
 * @brief Sets particles before the main calculation cycle.
 */
class Set_particles : public Command {
 public:
  using Filling_condition    = std::function<bool(int nx, int ny)>;
  using Density_profile      = std::function<double(int nx, int ny)>;
  using Coordinate_generator = std::function<vector2(int nx, int ny)>;
  using Momentum_generator   = std::function<vector3(const vector2& r)>;

  Set_particles(
    Particles* const particles,
    const Domain_geometry& circumscribing_rectangle,
    const Filling_condition& filling_condition,
    const Density_profile& density_profile,
    const Coordinate_generator& coordinate_generator,
    const Momentum_generator& momentum_generator);

  /**
   * @brief Traversing each cell in circumscribing rectangle it sets
   * a number of particles into the computational domain with some space
   * and momentum distributions.
   */
  void execute(int /* timestep */) override;

  /**
   * @brief Must be used once, so it is not needed after execution.
   */
  bool needs_to_be_removed(int t) const override {
    return true;
  }

 private:
  Particles* const particles_;

  Domain_geometry circumscribing_rectangle_;

  Filling_condition cell_should_be_filled_;
  Density_profile get_density_profile_;
  Coordinate_generator load_coordinate_;
  Momentum_generator load_momentum_;
};

#endif  // SRC_COMMAND_SET_PARTICLES_HPP
