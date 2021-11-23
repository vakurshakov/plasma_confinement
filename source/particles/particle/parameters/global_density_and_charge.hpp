#ifndef GLOBAL_CHARGE_AND_DENSITY_HPP 
#define GLOBAL_CHARGE_AND_DENSITY_HPP

//#################################################################################################

#include <utility>
#include <iostream>

#include "./global_densty.hpp"
#include "./global_charge.hpp"


/**
 * @brief This class adds density as well as charge to the
 *      major parameters of every particle in a sort.
 * 
 * @see ./major_parameters.hpp
 * @see ./density_is_global.hpp
 * @see ./charge_is_global.hpp
 */
class gDensity_gCharge : public gDensity, public gCharge {
public:
    // Default constructor.
    gDensity_gCharge() = default;

    /**
     * Constructor.
     * 
     * @param m  Mass of the particles in a sort.
     * @param n  Density of the particles in a sort. 
     * @param q  Charge of the particles in a sort.
     * @param Np Number of particles in cell.
     * @param Tx Temperature in x direction.
     * @param Ty Temperature in y direction.
     * @param Tz Temperature in z direction.
     * @param p0 Initial impulse (absolute value).
     * @param charge_cloud Width of the spline.
     * @param form_factor Shape of the macroparticle.
     */
	gDensity_gCharge(double n, double q, const gParameters& parameters)
    :   gParameters(parameters)
        {
            n_ = n;
            q_ = q;
        }
};


//#################################################################################################

#endif //GLOBAL_CHARGE_AND_DENSITY_HPP