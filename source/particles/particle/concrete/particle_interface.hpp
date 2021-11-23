#ifndef PARTICLE_INTERFACE_HPP 
#define PARTICLE_INTERFACE_HPP

//#################################################################################################

//#include <omp.h>
#include <iostream>
#include <memory>

#include "../point.hpp"
#include "../parameters/global_parameters.hpp"

/**
 * @brief This class provides a particle interface.
 *      Particle stores a point, that represents it,
 *      and a reference to a global parameters, that can
 *      be a one of the following: 
 *          1. Density and charge are global;
 *          2. Charge is global (hence, density is local);
 *          3. Density is global (hence, charge is local);
 *          4. Both density and charge are local.
 * 
 * @see ../point.hpp
 * @see ../parameters/global_parameters.hpp
 */
class IParticle {
public:
    virtual ~IParticle() = default;

    // Default constructor.
    IParticle() = delete;

    /**
     * Constructor.
     * @param point Point that represent a particle.
     */
    IParticle(Point&& point, const gParameters& parameters)
    :   point(std::move(point)), parameters(parameters) {}

    /**
     * @brief Gets a point that represent a particle.
     * @return Reference to the stored point. 
     */
    inline Point& get_point() { return point; }
    
    /**
     * @brief Gets a point that represent a particle.
     * @return Constant reference to the stored point. 
     */
    inline const Point& get_point() const { return point; }

    /**
     * @brief Gets a density of a particle.
     * @return Density of the partiparameterscle [in units of reference density]. 
     */
    virtual double n() const = 0;
    
    /**
     * @brief Gets a charge of a particle.
     * @return Charge of the particle [in units of elementary charge]. 
     */
    virtual double q() const = 0;

    /**
     * @brief Gets a mass of a particle.
     * @return Mass of the particle [in units of electron mass].
     *      Each particle in a sort has equal masses. 
     */
    double m() const { return parameters.m(); }

protected:
    Point point;
    const gParameters& parameters;
};


//#################################################################################################

#endif //PARTICLE_INTERFACE_HPP