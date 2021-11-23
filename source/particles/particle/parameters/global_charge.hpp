#ifndef GLOBAL_CHARGE_HPP 
#define GLOBAL_CHARGE_HPP

//#################################################################################################

#include <utility>
#include <iostream>

#include "./global_parameters.hpp"


/**
 * @brief This class adds charge to the major
 *      parameters of every particle in a sort.
 * 
 * @see ./global_parameters.hpp
 */
class gCharge : virtual public gParameters {
public:
    // Default constructor.
    gCharge() = default;

    /**
     * Constructor.
     * 
     * @param m  Mass of the particles in a sort.
     * @param q  Charge of the particles in a sort.
     * @param Np Number of particles in cell.
     * @param Tx Temperature in x direction.
     * @param Ty Temperature in y direction.
     * @param Tz Temperature in z direction.
     * @param p0 Initial impulse (absolute value).
     * @param charge_cloud Width of the spline.
     * @param form_factor Shape of the macroparticle.
     */
	gCharge(double q, const gParameters& parameters)
    :   gParameters(parameters),
        q_(q) {}

    // Charge getter.
	double q() const { return q_; }

protected:
	double q_;
};


//#################################################################################################

#endif //GLOBAL_CHARGE_HPP