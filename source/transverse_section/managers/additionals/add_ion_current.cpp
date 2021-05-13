#include "../particles.hpp"

void Particles::add_ion_current(int t)
{
	if ( t < TINJ ) {	n_ += ni/TINJ;	} 
}