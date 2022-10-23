#include "copy_coordinates.hpp"
#include "../particles/particle/particle.hpp"
  
// А как быть, если плотности сортов разные?  
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
        double x = particle.point.x();
        double y = particle.point.y();
    
        double px, py, pz;
        do
        {
		    load_impulse(x, y, mass, Tx, Ty, Tz, p0, &px, &py, &pz);
        }
        while (std::isinf(px) || std::isinf(py) || std::isinf(pz));
        
        particles_that_copies->add_particle(Point({x, y}, {px, py, pz}));    
	}
}
