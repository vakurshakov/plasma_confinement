#ifndef COMMAND_IONIZE_PARTICLES_HPP
#define COMMAND_IONIZE_PARTICLES_HPP

#include "command.hpp"

#include "src/pch.h"
#include "src/particles/particles.hpp"
#include "src/file_writers/bin_file.hpp"

/**
 * @brief
 * Command that sets fixed number of particles
 * in the computational domain each time step.
 */
class Ionize_particles : public Command {
 public:
  using Random_coordinate_generator = std::function<void(double& x, double& y)>;
  using Density_profile = std::function<double(int nx, int ny)>;
  using Momentum_generator = std::function<vector3(const vector2& r)>;

  Ionize_particles(
    Particles* const ionized,
    Particles* const ejected,
    int injection_start,
    int injection_end,
    int per_step_particles_num,
    const Random_coordinate_generator& set_point_of_birth,
    const Density_profile& get_probability,
    const Momentum_generator& load_momentum_i,
    const Momentum_generator& load_momentum_e);

  /**
   * @brief Loads the number of particles (per_step_particles_num)
   * in pairs into a computational domain with some space and
   * momentum distributions.
   *
   * @param t Outer time step to start from injection_start.
   */
  void execute(int t) override;

  bool needs_to_be_removed(int t) const override {
    return t >= injection_end_;
  }

private:
  Particles* const ionized_;
  Particles* const ejected_;

  int injection_start_;
  int injection_end_;
  int per_step_particles_num_;

  Random_coordinate_generator set_point_of_birth_;
  Density_profile get_probability_;
  Momentum_generator load_momentum_i_;
  Momentum_generator load_momentum_e_;

  BIN_File ionized_energy_;
  BIN_File ejected_energy_;
};

#endif // COMMAND_IONIZE_PARTICLES_HPP
