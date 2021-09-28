#include "add_ionization.hpp"

#include <omp.h>

#include "particles.hpp"


double uniform_density(int nx, int ny) { return ni; }


void Ionization::process(Particles* const ionized, Particles* const lost) {
    const int Np = ionized->parameters_.Np();
	const double n = ionized->parameters_.n();
    const double mass = ionized->parameters_.m();
    const double Tx = ionized->parameters_.Tx();
    const double Ty = ionized->parameters_.Ty();
    const double Tz = ionized->parameters_.Tz();
    const double p0 = ionized->parameters_.p0();

    // не оптимально, конечно, такой цикл делать здесь
	#pragma omp parallel for num_threads(THREAD_NUM)
    for (int nx = 0; nx < SIZE_X; ++nx) {
    for (int ny = 0; ny < SIZE_Y; ++ny) {
        
        if ( this_is_config_cell_(nx, ny) ) {
			double local_density = this->check_density(ionized, nx, ny);	
            double shaped_density = density_shape_(nx, ny);

			int err = 0;
    		for (int i = 0; i < Np + err; ++i) {
				if ( local_density < shaped_density ) {
					local_density +=  n/Np; 
	
					double x, y;
    				fill_the_cell_(i, Np, nx, ny, &x, &y);

    				double px, py, pz;
    				load_impulse_(x, y, mass, Tx, Ty, Tz, p0, &px, &py, &pz);

    				if ( std::isinf(px) || std::isinf(py) || std::isinf(pz)) { 
    					++err;
    					continue;
    				}
    				else {
						Point ionized_particle({x, y}, {px, py, pz});
                	    Point lost_particle({x, y}, {-px, -py, -pz});
    					
						#pragma omp critical
						ionized->points_.push_back(std::move(ionized_particle));
						
						#pragma omp critical
                	    lost->points_.push_back(std::move(lost_particle));
    				}
				}
				else {
					break;
				}
    		}
    	}
    }}	
}


// TODO: optimization!
double Ionization::check_density(Particles* const ionized, int nx, int ny)
{
	const int Np = ionized->parameters_.Np(); 
	const double n = ionized->parameters_.n();

	// this will work only with a small numbers of particles
	double local_density = 0;
	for (auto& point : ionized->points_) {
		const vector2 r = point.r();

		int mx = int(roundf(r.x/dx));
		int my = int(roundf(r.y/dy));
		if ( mx == nx && my == ny ) {
			local_density += n/Np;
		}
	}
	return local_density;
}