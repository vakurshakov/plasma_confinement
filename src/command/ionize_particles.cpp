#include "ionize_particles.hpp"

#include "../particles/particle/particle.hpp"
#include "../managers/random_number_generator.hpp"
#include "../constants.h"


void set_point_on_annulus(double* x, double* y)
{
	// To close the interval of the real distribution we use std::nextafter(double from, double to)
	static auto distribution = std::uniform_real_distribution(0., std::nextafter(1., 2.));
	
	double ra = r_larm - dr; 
	double rb = r_larm + dr;
	double r = sqrt(ra * ra + (rb * rb - ra * ra) *
		distribution(Random_generator::get()));

	double phi = 2 * M_PI * distribution(Random_generator::get());

	*x = 0.5 * SIZE_X * dx + r * cos(phi);
	*y = 0.5 * SIZE_Y * dy + r * sin(phi);
}

void set_point_on_circle_segment(double* x, double* y)
{
	static const double r_plasma = (r_larm + dr) * r_prop;
	static const double center_x = 0.5 * SIZE_X * dx;
	static const double center_y = 0.5 * SIZE_Y * dy;
	static const double plasmas_left_x = center_x - r_plasma;
	static const double plasmas_height = sqrt(r_plasma * r_plasma - (r_plasma - dr) * (r_plasma - dr));

	// To close the interval of the real distribution we use std::nextafter(double from, double to)
	static auto x_distribution = std::uniform_real_distribution(plasmas_left_x, plasmas_left_x + dr);
	static auto y_distribution = std::uniform_real_distribution(center_y - plasmas_height, center_y + plasmas_height);

	double r;

	do
	{
		*x = x_distribution(Random_generator::get());
		*y = y_distribution(Random_generator::get());

		double centered_x = *x - center_x;
		double centered_y = *y - center_y;
		
		r  = sqrt(centered_x * centered_x + centered_y * centered_y);
	}
	while(r > r_plasma);
}


double uniform_probability(double _1, double _2)
{
	return 1.;
}

double get_cosine_probability(double x, double y)
{  
	static const double r_plasma = (r_larm + dr) * r_prop;
	static const double center_y = 0.5 * SIZE_Y * dy;

	static const double height = sqrt(r_plasma * r_plasma - (r_plasma - dr) * (r_plasma - dr));

	return 0.5 * (1. + cos(M_PI * (y - center_y) / height)) / (height + 1.);
}

#if density_beam_profile_is_set
double density_beam_profile(double x, double y)
{
	x -= 0.5 * SIZE_X * dx;
	y -= 0.5 * SIZE_Y * dy;

	const double r = sqrt(x * x + y * y) - r_larm;
	static const double sigma2 = dr * dr / 6.25; 

	double ans = 0.;

	if (r * r <  dr * dr)
		ans = ni * exp(- 0.5 * r * r / sigma2) / sqrt(2. * M_PI * sigma2);

	return ans;
}
#endif


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
    const double mi = ionized->get_parameters().m();
    const double Ti_x = ionized->get_parameters().Tx();
    const double Ti_y = ionized->get_parameters().Ty();
    const double Ti_z = ionized->get_parameters().Tz();
    const double pi_0 = ionized->get_parameters().p0();
 
	const double ml = lost->get_parameters().m();
    const double Tl_x = lost->get_parameters().Tx();
    const double Tl_y = lost->get_parameters().Ty();
    const double Tl_z = lost->get_parameters().Tz();
    const double pl_0 = lost->get_parameters().p0();

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
			double pl_x, pl_y, pl_z;
			do 
			{
				load_impulse(x, y, mi, Ti_x, Ti_y, Ti_z, pi_0, &pi_x, &pi_y, &pi_z);
				load_impulse(x, y, ml, Tl_x, Tl_y, Tl_z, pl_0, &pl_x, &pl_y, &pl_z);
			} while (
				std::isinf(pi_x) || std::isinf(pi_y) || std::isinf(pi_z) ||
				std::isinf(pl_x) || std::isinf(pl_y) || std::isinf(pl_z));


			#pragma omp critical
			ionized->add_particle(Point({x, y}, {pi_x, pi_y, pi_z})
			#if density_beam_profile_is_set
				, density_beam_profile(x, y)
			#endif
			);

			#pragma omp critical
        	lost->add_particle(Point({x, y}, {pl_x, pl_y, pl_z})
			#if density_beam_profile_is_set
				, density_beam_profile(x, y)
			#endif
			);
		}
	}
}