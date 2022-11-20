#ifndef COMMAND_COPY_COORDINATES_HPP
#define COMMAND_COPY_COORDINATES_HPP

#include "command.hpp"

#include "src/pch.h"
#include "src/particles/particles.hpp"
#include "src/particles/particles_load.hpp"

/**
 * @brief Command to copy coordinates from another particles.
 */
class Copy_coordinates : public Command {
 public:

  /**
   * @brief Constructor of the command.
   *
   * @param particles_copy_to Pointer on particles that will copy coordinates.
   * @param particles_copy_from Pointer on particles which will share coordinates.
   * @param load_impulse Impulse distribution function.
   */
  Copy_coordinates(
    Particles* const particles_copy_to,
    const Particles* const particles_copy_from,
    const impulse_loader& load_impulse);

  /**
   * @brief Passes through every particle of particles_copy_from vector,
   *  copies coordinates from it to particles_copy_to and
   *  gives to this points another impulse.
   */
  void execute(int /* timestep */) const override;

 private:
  Particles* const particles_copy_to_;
  const Particles* const particles_copy_from_;

  impulse_loader load_impulse_;
};

#endif // COMMAND_COPY_COORDINATES_HPP