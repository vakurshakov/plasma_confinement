#ifndef COMMAND_COPY_COORDINATES_HPP
#define COMMAND_COPY_COORDINATES_HPP

#include "command.hpp"

#include "src/pch.h"
#include "src/particles/particles.hpp"

/**
 * @brief Copies coordinates from another particles.
 */
class Copy_coordinates : public Command {
 public:
  using Momentum_generator = std::function<vector3(const vector2& r)>;

  Copy_coordinates(
    Particles* const particles_copy_to,
    const Particles* particles_copy_from,
    const Momentum_generator& load_momentum);

  /**
   * @brief Passes through particles_copy_from vector and
   * copies coordinates from it to particles_copy_to,
   * giving to this points momentum from load_momentum.
   */
  void execute(int /* timestep */) override;

  /**
   * @brief Must be used once, so it is not needed after execution.
   */
  bool needs_to_be_removed(int t) const override {
    return true;
  }

 private:
  Particles* const particles_copy_to_;
  const Particles* particles_copy_from_;

  Momentum_generator load_momentum_;
};

#endif // COMMAND_COPY_COORDINATES_HPP
