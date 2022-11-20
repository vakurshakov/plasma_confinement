#ifndef COMMAND_IONIZE_PARTICLES_HPP
#define COMMAND_IONIZE_PARTICLES_HPP

#include "command.hpp"

#include "src/pch.h"
#include "src/particles/particles.hpp"
#include "src/particles/particles_load.hpp"

/**
 * @brief
 * Command that sets fixed number of particles
 * in the computational domain each time step.
 */
class Ionize_particles : public Command {
public:

  /**
   * @brief 
   * Constructor of the command.
   *
   * @param ionized Particles that will be ionized.
   * @param ejected Particles to conserve charge.
   * @param per_step_particles_num Array of amounts
   *   of particles that will be loaded on time step t.
   * @param set_point_of_birth Function that calculates birth point.
   * @param get_probability Gets probability of birth in the point.
   * @param load_impulse Impulse distribution function.
   */
  Ionize_particles(
    Particles* const ionized, Particles* const ejected,
    const std::vector<size_t>& per_step_particles_num,
    std::function<void(double* x, double* y)>&& set_point_of_birth,
    std::function<double(double x, double y)>&& get_probability,
    const impulse_loader& load_impulse);

  /**
   * @brief
   * On a concrete time step t, it loads the number of
   * particles (according to per_step_particles_num) in
   * pairs into a computational domain with some space
   * and momentum distributions.
   *
   * @param t Outer time step to load a concrete
   * amount of particles on that step.
   */
  void execute(int t) const override;

  /**
   * @brief
   * Checks whether command needs to be
   * removed from the command list or not.
   *
   * @param t Outer time step, base of decision.
   *
   * @return
   * "true" if the time is greater or equal to
   * the time of injection, "false" otherwise.
   */
  bool needs_to_be_removed(int t) const override {
    return !(t < config::INJECTION_TIME);
  }

private:
  Particles* const ionized;
  Particles* const ejected;

  std::vector<size_t> per_step_particles_num;

  std::function<void(double* x, double* y)> set_point_of_birth;
  std::function<double(double x, double y)> get_probability;
  impulse_loader load_impulse;
};


/**
 * @brief
 * Probability function that gives the probability
 * of birth at a particular point.
 *
 * @param x Coordinate of a point in space.
 * @param y Coordinate of a point in space.
 *
 * @return Always return 1.0
 */
double uniform_probability(double x, double y);

/**
 * @brief
 * Specifies a time distribution of birth particles.
 *
 * @param t_inj The number of time steps during which the particles will be loaded.
 * @param total_particles_num Total amount of particles that will be loaded.
 *
 * @return Vector of amounts of particles to be set at specific time.
 */
std::vector<size_t> set_time_distribution(size_t t_inj, size_t total_particles_num);

#endif // COMMAND_IONIZE_PARTICLES_HPP