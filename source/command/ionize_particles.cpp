#include "ionize_particles.hpp"

#include <cmath> // for nextafter(...) isinf(double)

#include "../particles/particles.hpp"
#include "../managers/random_number_generator.hpp"


void set_point_on_annulus(double* x, double* y)
{
	// To close the interval of the real distribution we use std::nextafter(double from, double to)
	static auto distribution = std::uniform_real_distribution(0., std::nextafter(1., 2.));
	
	double ra = r_larm - dr; 
	double rb = r_larm + dr;
	double r = sqrt(ra*ra + (rb*rb - ra*ra)*distribution(Random_generator::get()));

	double phi = 2*M_PI*distribution(Random_generator::get());

	*x = SIZE_X/2*dx + r*cos(phi);
	*y = SIZE_Y/2*dy + r*sin(phi);
}


double uniform_probability(double _1, double _2)
{
	return 1.;
}


std::vector<size_t> set_time_distribution(int t_inj, size_t total_number_of_particles)
{
	std::vector<size_t> array_of_particles_to_load(t_inj);
	std::uniform_int_distribution distribution(0, t_inj-1);

	for (size_t n = 0; n < total_number_of_particles; ++n) {
		array_of_particles_to_load[ distribution(Random_generator::get()) ] += 1;
	}

	return array_of_particles_to_load;
}


void Ionize_particles::execute(int t) const {
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

	// To close the interval of the real distribution we use std::nextafter(double from, double to)
	auto distribution = std::uniform_real_distribution(0., std::nextafter(1., 2.));

	int err = 0;
	for (size_t i = 0; i < amount_of_particles_to_load_per_step[t] + err; ++i) {
		
		double x, y;
		set_point_of_birth(&x, &y);
		
		if ( distribution(Random_generator::get()) > get_probability(x, y) ) {
			++err;
			continue;
		}
		else {
			double pi_x, pi_y, pi_z;
			load_impulse(x, y, mi, Ti_x, Ti_y, Ti_z, pi_0, &pi_x, &pi_y, &pi_z);

			double pl_x, pl_y, pl_z;
			load_impulse(x, y, ml, Tl_x, Tl_y, Tl_z, pl_0, &pl_x, &pl_y, &pl_z);

			if ( std::isinf(pi_x) || std::isinf(pi_y) || std::isinf(pi_z) ||
				 std::isinf(pl_x) || std::isinf(pl_y) || std::isinf(pl_z) ) { 
				++err;
				continue;
			}
			else {
				#pragma omp critical
				ionized->points_.emplace_back(Point({x, y}, {pi_x, pi_y, pi_z}));
				
				#pragma omp critical
        	    lost->points_.emplace_back(Point({x, y}, {pl_x, pl_y, pl_z}));
			}
		}
	}
}