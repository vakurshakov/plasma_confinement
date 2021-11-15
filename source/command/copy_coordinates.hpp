#ifndef COMMAND_COPY_COORDINATES_HPP
#define COMMAND_COPY_COORDINATES_HPP

//#######################################################################################

#include "command.hpp"

#include <vector>
#include <string>

#include "../particles/particles.hpp"

/**
 * @brief Command to copy coordinates from another particles.
 */
class Copy_coordinates : public Command {
public:
	
    /**
     * @brief Constructor of the command.
     * 
     * @param particles_that_copies Pointer on particles that will copy coordinates. 
     * @param particles_to_copy Pointer on particles which will share coordinates.
     * @param load_impulse Impulse distribution function.
     */
	Copy_coordinates(
		Particles* const particles_that_copies,
        const Particles* const particles_to_copy,
        std::function<void(double x, double y,
		    double mass, double Tx, double Ty, double Tz,
		    double p0, double* px, double* py, double* pz)> load_impulse)
		:	particles_that_copies(particles_that_copies),
            particles_to_copy(particles_to_copy),
            load_impulse(load_impulse) {};

    /**
     * @brief Passes through every particle of particles_to_copy vector,
     *      copies coordinates from it to particles_that_copies and 
     *      gives to this points another impulse.
     * 
     * @param _ Placeholder for the time, pass 0.
     */
	void execute(int _) const override;

private:
	Particles* const particles_that_copies;
    const Particles* const particles_to_copy;
    
    std::function<void(double x, double y,
	    double mass, double Tx, double Ty, double Tz,
	    double p0, double* px, double* py, double* pz)> load_impulse;    
};

//#######################################################################################

#endif // COMMAND_COPY_COORDINATES_HPP