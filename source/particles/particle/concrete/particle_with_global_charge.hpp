#ifndef PARTICLE_WITH_GLOBAL_CHARGE_HPP 
#define PARTICLE_WITH_GLOBAL_CHARGE_HPP

//#################################################################################################

#include "./particle_interface.hpp"
#include "../point.hpp"
#include "../parameters/global_charge.hpp"
 

/**
 * Particle that have global charge.
 * 
 * @see ./particle_interface.hpp
 * @see ../parameters/global_charge.hpp   
 */
class gCharge_Particle : public IParticle {
public:
    gCharge_Particle() = delete;
    
    /**
     * Constructor.
     * 
     * @param point Point representing the particle.
     * @param n Density of the particle.
     * @param parameters Reference to global particle's parameters.
     * 
     * @warning It is your responsibility to set correct
     *      gParameters = gDensity_gCharge.
     * 
     * @todo gParameters checking.
     */
    gCharge_Particle(
    Point&& point, double n, const gParameters& parameters)
    :   IParticle(std::move(point), parameters), n_(n) {}

    /**
     * @brief Gets density of a particle.
     * @return Density of the particle in dimensionless units. 
     */
    double n() const override { return n_; }
    
    /**
     * @brief Gets charge of a particle.
     * @return Charge of the particle in dimensionless units. 
     */
    double q() const override { return parameters.q(); }

private:
    const double n_;
};


//#################################################################################################

#endif //PARTICLE_WITH_GLOBAL_CHARGE_HPP