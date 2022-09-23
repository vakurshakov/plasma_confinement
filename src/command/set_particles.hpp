#ifndef SRC_COMMAND_SET_PARTICLES_HPP
#define SRC_COMMAND_SET_PARTICLES_HPP

#include "command.hpp"

#include "src/pch.h"
#include "src/particles/particles.hpp"

/**
 * @brief Command that sets particles before the main calculation cycle.
 */
class Set_particles : public Command {
 public:
  /**
   * @brief Constructor of the command.
   *
   * @param particles Pointer on particles to be set.
   * @param number_of_particles_to_load Precalculated shape of a resulted vector.
   * @param this_is_config_cell Function that indicates a cell
   * is in configuration when the cells pass through each one in domain.
   * @param load_impulse Impulse distribution function.
   * @param fill_the_cell Way of filling particles in the cell.
   *
   * @warning Total number of particles may be not equal to
   * @a number_of_particles_to_load, each cell that is in configuration
   * fills with Np particles.
   */
  Set_particles(
    Particles* const particles,
    std::size_t number_of_particles_to_load,
    std::function<bool(int cell_number_nx, int cell_number_ny)> this_is_config_cell,
    std::function<void(double x, double y,
      double mass, double Tx, double Ty, double Tz,
      double p0, double* px, double* py, double* pz)> load_impulse,
    std::function<void(int sequential_number, int Np,
      int cell_number_nx, int cell_number_ny,
      double* x, double* y)> fill_the_cell )
    :	particles(particles),
      number_of_particles_to_load(number_of_particles_to_load),
      this_is_config_cell(this_is_config_cell),
      load_impulse(load_impulse),
      fill_the_cell(fill_the_cell) {};

  /**
   * @brief Passes through each cell in the domain and if the
   * cell is in config, it puts there Np particles with
   * chosen impulse distribution and way of filling.
   */
  void execute(int /* timestep */) const override;

 private:
  Particles* const particles;

  std::size_t number_of_particles_to_load;

  std::function<bool(int cell_number_nx, int cell_number_ny)> this_is_config_cell;

  std::function<void(double x, double y,
    double mass, double Tx, double Ty, double Tz,
    double p0, double* px, double* py, double* pz)> load_impulse;

  std::function<void(int sequential_number, int Np,
    int cell_number_nx, int cell_number_ny,
    double* x, double* y)> fill_the_cell;
};

#endif  // SRC_COMMAND_SET_PARTICLES_HPP
