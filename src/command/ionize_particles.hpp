#ifndef COMMAND_IONIZE_PARTICLES_HPP
#define COMMAND_IONIZE_PARTICLES_HPP

//#######################################################################################

#include "command.hpp"

#include "src/pch.h"
#include "../particles/particles.hpp"

/**
 * @brief Command that sets fixed number of particles (in pairs)
 * 		in the computational domain each time step.
 * 
 * @todo Ionziation process with more than one buffer particle.
 */
class Ionize_particles : public Command {
public:

	/**
	 * Constructor of the command.
	 * 
	 * @param ionized Pointer to particles that will be ionized.
	 * @param lost 	  Pointer to buffer particles to conserve charge. 
	 * @param number_of_particles_to_load_per_step Array of amounts
	 * 		of particles that will be loaded on time step t. 
     * @param set_point_of_birth Function that calculates birth point.
     * @param get_probability Gets probability of birth in the point.
     * @param load_impulse Impulse distribution function.
	 * 
	 * @todo Set the command in the builder of particles
	 */
	Ionize_particles(
		Particles* const ionized, Particles* const lost,
		const std::vector<size_t>& amount_of_particles_to_load_per_step,
        std::function<void(double* x, double* y)>&& set_point_of_birth,
        std::function<double(double x, double y)>&& get_probability,
        std::function<void(double x, double y,
		  double mass, double Tx, double Ty, double Tz,
          double p0, double* px, double* py, double* pz)>&& load_impulse )
			:	ionized(ionized), lost(lost),
				amount_of_particles_to_load_per_step(amount_of_particles_to_load_per_step),
                set_point_of_birth(std::move(set_point_of_birth)),
                get_probability(std::move(get_probability)),
                load_impulse(std::move(load_impulse)) {};

	/**
	 * @brief On a concrete time step t, it loads the number of particles
	 * 		(according to amount_of_particles_to_load_per_step) in pairs into
	 * 		a computatoinal domain with some space and momentum distributions. 
	 * 
	 * @param t Outer time step to load a concrete
	 * 		amount of particles on that step.
	 */
	void execute(int t) const override;

	/**
	 * @brief Checks whether command needs to be removed
	 * 		from the command list or not.
	 * 
	 * @param t Outer time step, base of desicion.
	 * 
	 * @return True if the time is greater or equal to
	 * 		the time of injection, false otherwise. 
	 */
	bool needs_to_be_removed(int t) const override {
		return !(t < TINJ);
	}

private:
	Particles* const ionized;
	Particles* const lost;
	
	std::vector<size_t> amount_of_particles_to_load_per_step;
	
	std::function<void(double* x, double* y)> set_point_of_birth;
	
	std::function<double(double x, double y)> get_probability;
	
	std::function<void(double x, double y,
	  double mass, double Tx, double Ty, double Tz,
	  double p0, double* px, double* py, double* pz)> load_impulse;
};


/**
 * @brief Sets the input coordinates to a random point
 * 		from the annulus with concrete smaller and larger radius.
 * 
 * @warning Setter uses global r_larm, dr. So that smaller radius
 * 		is r_larm - dr, and the larger is r_larm + dr.
 */
void set_point_on_annulus(double* x, double* y);

/**
 * @brief Sets (x,y) randomly on the intersection of x < dr and r < r_plasma - dr. 
 * 
 * @warning Setter uses global r_larm, r_prop, dr.
 */
void set_point_on_circle_segment(double* x, double* y);

/**
 * @brief Probability function that gives the probablity
 * 		of birth at a particular point. 
 * 
 * @param x Coordinate of a point in space.
 * @param y Coordinate of a point in space.
 * 
 * @return 1.0
 */
double uniform_probability(double x, double y);

double get_cosine_probability(double x, double y);

/**
 * @brief Specifies a time distribution of birth particles.
 * 
 * @param t_inj The number of time steps during which the particles will be loaded.
 * @param total_number_of_particles Total amount of particles that will be loaded.
 * 
 * @return Vector of amounts of particles to be set at specific time.
 */
std::vector<size_t> set_time_distribution(int t_inj, size_t total_number_of_particles);

//#######################################################################################

#endif // COMMAND_IONIZE_PARTICLES_HPP