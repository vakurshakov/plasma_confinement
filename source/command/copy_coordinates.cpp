#include "copy_coordinates.hpp"

#include <cmath> // for isinf(double)

#include "../particles/particle/concrete/particle_with_global_density_and_charge.hpp"


void Copy_coordinates::execute(int _) const
{
    const double mass = particles_that_copies->get_parameters().m();
    const double Tx   = particles_that_copies->get_parameters().Tx();
    const double Ty   = particles_that_copies->get_parameters().Ty();
    const double Tz   = particles_that_copies->get_parameters().Tz();
    const double p0   = particles_that_copies->get_parameters().p0();
	
	particles_that_copies->particles_.reserve(particles_to_copy->particles_.size());

	for (const auto& particle : particles_to_copy->particles_)
    {
        double x = particle->get_point().x();
        double y = particle->get_point().y();
    
        double px, py, pz;
        do
        {
		    load_impulse(x, y, mass, Tx, Ty, Tz, p0, &px, &py, &pz);
        }
        while (std::isinf(px) || std::isinf(py) || std::isinf(pz));
        
        particles_that_copies->particles_.emplace_back(
            std::make_unique<gDensity_gCharge_Particle>(
                Point({x, y}, {px, py, pz}),
                particles_that_copies->get_parameters()
            )
        );    
	}
}
