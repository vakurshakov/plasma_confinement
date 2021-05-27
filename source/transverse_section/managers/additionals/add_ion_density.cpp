#include "../particles.hpp"

void Particles::add_ion_density(int t)
{
	if ( t <= TINJ ) {
		n_ = ni*sin(0.5*M_PI*double(t)/double(TINJ));
	}
}