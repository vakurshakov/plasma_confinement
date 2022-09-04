#include "set_particles.hpp"

#include "../particles/particle/particle.hpp"

 
void Set_particles::execute(int _) const
{
    const double mass = particles->get_parameters().m();
    const double Tx   = particles->get_parameters().Tx();
    const double Ty   = particles->get_parameters().Ty();
    const double Tz   = particles->get_parameters().Tz();
    const double p0   = particles->get_parameters().p0();
	const int    Np   = particles->get_parameters().Np();

	particles->particles_.reserve( number_of_particles_to_load );

	for (int nx = 0; nx < SIZE_X; ++nx) {
	for (int ny = 0; ny < SIZE_Y; ++ny) {
		
		if ( this_is_config_cell(nx, ny) )
        {
			for (int i = 0; i < Np; ++i)
            {
				double x, y;
				fill_the_cell(i, Np, nx, ny, &x, &y);

				double px, py, pz;
				do
                {
                    load_impulse(x, y, mass, Tx, Ty, Tz, p0, &px, &py, &pz);
                }
                while (std::isinf(px) || std::isinf(py) || std::isinf(pz));

				particles->add_particle(Point({x, y}, {px, py, pz}));
			}
		}
	}}
}