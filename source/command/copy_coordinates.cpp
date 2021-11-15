#include "copy_coordinates.hpp"

#include <cmath> // for isinf(double)


void Copy_coordinates::execute(int _) const
{
    const double mass = particles_that_copies->get_parameters().m();
    const double Tx   = particles_that_copies->get_parameters().Tx();
    const double Ty   = particles_that_copies->get_parameters().Ty();
    const double Tz   = particles_that_copies->get_parameters().Tz();
    const double p0   = particles_that_copies->get_parameters().p0();
	
	particles_that_copies->points_.reserve(particles_to_copy->points_.size());

	for (const auto& point : particles_to_copy->points_)
    {
        double x = point.x();
        double y = point.y();
    
        double px, py, pz;
        do
        {
		    load_impulse(x, y, mass, Tx, Ty, Tz, p0, &px, &py, &pz);
        }
        while (std::isinf(px) || std::isinf(py) || std::isinf(pz));
        
        particles_that_copies->points_.emplace_back(Point({x, y}, {px, py, pz}));    
	}
}
