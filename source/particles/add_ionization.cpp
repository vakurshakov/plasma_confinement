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
    const double mass = ionized->parameters_.m();
    const double Tx = ionized->parameters_.Tx();
    const double Ty = ionized->parameters_.Ty();
    const double Tz = ionized->parameters_.Tz();
    const double p0 = ionized->parameters_.p0();
 
	int err = 0;
	for (int i = 0; i < array_of_particles_to_load_[t] + err; ++i) {
		
		double x, y;
		set_point_of_birth_(&x, &y);
		
		if ( frand() > get_probability(x, y) ) {
			++err;
			continue;
		}
		else {
			double px, py, pz;
			load_impulse_(x, y, mass, Tx, Ty, Tz, p0, &px, &py, &pz);

			if ( std::isinf(px) || std::isinf(py) || std::isinf(pz)) { 
				++err;
				continue;
			}
			else {
				#pragma omp critical
				ionized->points_.emplace_back(Point({x, y}, {px, py, pz}));
				
				#pragma omp critical
        	    lost->points_.emplace_back(Point({x, y}, {-px, -py, -pz}));
			}
		}
	}
}