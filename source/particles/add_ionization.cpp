#include "add_ionization.hpp"

#include <omp.h>
#include <cmath>
#include <random>

#include "particles.hpp"
#include "particles_load.hpp" // for frand()


double uniform_probability(double x, double y) { return 1.; }


void set_point_on_circle(double* x, double* y)
{
	double ra = r_larm - dr; 
	double rb = r_larm + dr;
	double r = sqrt(ra*ra + (rb*rb - ra*ra)*frand());

	double phi = 2*M_PI*frand();

	*x = SIZE_X/2*dx + r*cos(phi);
	*y = SIZE_Y/2*dy + r*sin(phi);
}


std::vector<int> set_time_distribution(int t_inj, size_t total_number_of_particles)
{
	std::vector<int> array_of_particles_to_load(t_inj);
	std::minstd_rand gen(42);
	std::uniform_int_distribution<int> distribution(0, (t_inj-1));

	for (size_t n = 0; n < total_number_of_particles; ++n) {
		array_of_particles_to_load[ distribution(gen) ] += 1;
	}

	return array_of_particles_to_load;
}


void Ionization::process(Particles* const ionized, Particles* const lost, int t) {
    const double mi = ionized->parameters_.m();
    const double Ti_x = ionized->parameters_.Tx();
    const double Ti_y = ionized->parameters_.Ty();
    const double Ti_z = ionized->parameters_.Tz();
    const double pi_0 = ionized->parameters_.p0();
 
	const double ml = lost->parameters_.m();
    const double Tl_x = lost->parameters_.Tx();
    const double Tl_y = lost->parameters_.Ty();
    const double Tl_z = lost->parameters_.Tz();
    const double pl_0 = lost->parameters_.p0();

	int err = 0;
	for (int i = 0; i < array_of_particles_to_load_[t] + err; ++i) {
		
		double x, y;
		set_point_of_birth_(&x, &y);
		
		if ( frand() > get_probability(x, y) ) {
			++err;
			continue;
		}
		else {
			double pi_x, pi_y, pi_z;
			load_impulse_(x, y, mi, Ti_x, Ti_y, Ti_z, pi_0, &pi_x, &pi_y, &pi_z);

			double pl_x, pl_y, pl_z;
			load_impulse_(x, y, ml, Tl_x, Tl_y, Tl_z, pl_0, &pl_x, &pl_y, &pl_z);

			if ( std::isinf(pi_x) || std::isinf(pi_y) || std::isinf(pi_z) ||
				 std::isinf(pl_x) || std::isinf(pl_y) || std::isinf(pl_z) ) { 
				++err;
				continue;
			}
			else {
				#pragma omp critical
				ionized->points_.emplace_back(Point({x, y}, {pi_x, pi_y, pi_z}));
				
				#pragma omp critical
        	    lost->points_.emplace_back(Point({x, y}, {-pl_x, -pl_y, -pl_z}));
			}
		}
	}
}