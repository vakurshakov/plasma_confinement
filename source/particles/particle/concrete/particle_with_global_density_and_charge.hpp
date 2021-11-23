#ifndef PARTICLE_WITH_GLOBAL_DENSITY_AND_CHARGE_HPP 
#define PARTICLE_WITH_GLOBAL_DENSITY_AND_CHARGE_HPP

//#################################################################################################

#include "./particle_interface.hpp"
#include "../parameters/global_density_and_charge.hpp"
#include "../point.hpp"
 

/**
 * Particle that have global density and charge.
 * 
 * @see ./particle_interface.hpp
 * @see ../parameters/global_density_and_charge.hpp   
 */
class gDensity_gCharge_Particle : public IParticle {
public:
    gDensity_gCharge_Particle() = delete;

    /**
     * Constructor.
     * 
     * @param point Point representing the particle.
     * @param parameters Reference to global particle's parameters.
     * 
     * @warning It is your responsibility to set correct
     *      gParameters = gDensity_gCharge.
     * 
     * @todo gParameters checking.
     */
    gDensity_gCharge_Particle(
    Point&& point, const gParameters& parameters)
    :   IParticle(std::move(point), parameters) {}


    /**
     * @brief Gets density of a particle.
     * @return Density of the particle in dimensionless units. 
     */
    double n() const override { return parameters.n(); }
    
    /**
     * @brief Gets charge of a particle.
     * @return Charge of the particle in dimensionless units. 
     */
    double q() const override { return parameters.q(); }
};


//#################################################################################################

#endif //PARTICLE_WITH_GLOBAL_DENSITY_AND_CHARGE_HPP