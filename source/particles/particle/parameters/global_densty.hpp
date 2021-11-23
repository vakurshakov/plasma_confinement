#ifndef GLOBAL_DENSITY_HPP 
#define GLOBAL_DENSITY_HPP

//#################################################################################################

#include <utility>

#include "./global_parameters.hpp"


/**
 * @brief This class adds density to the global
 *      parameters of every particle in a sort.
 * 
 * @see ./global_parameters.hpp
 */
class gDensity : virtual public gParameters {
public:
    // Default constructor.
    gDensity() = default;
 
    /**
     * Constructor.
     * 
     * @param m  Mass of the particles in a sort.
     * @param n  Density of the particles in a sort.
     * @param Np Number of particles in cell.
     * @param Tx Temperature in x direction.
     * @param Ty Temperature in y direction.
     * @param Tz Temperature in z direction.
     * @param p0 Initial impulse (absolute value).
     * @param charge_cloud Width of the spline.
     * @param form_factor Shape of the macroparticle.
     */
	gDensity(double n, const gParameters& parameters)
		:   gParameters(parameters),
            n_(n) {}

    // Density getter.
	double n() const override { return n_; }

protected:
	double n_;
};


//#################################################################################################

#endif //GLOBAL_DENSITY_HPP