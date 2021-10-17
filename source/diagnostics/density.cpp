#include "density.hpp"

#include <string>
#include <vector>

#include "../particles/particle/particle_parameters.hpp"
#include "../particles/particle/point.hpp"
#include "../constants.h"


density::density(std::string directory_path)
	: Particles_diagnostic(directory_path, "density")
{
	dens_.reserve(SIZE_X*SIZE_Y);
	this->clear();
	
	this->save_parameters(directory_path);
}

void density::save_parameters(std::string directory_path)
{
	std::ofstream diagnostic_parameters_((directory_path + "/parameters.txt").c_str(), std::ios::out);
	diagnostic_parameters_ << "#TIME dt DTS" << std::endl;
	diagnostic_parameters_ << TIME << " " << dt << " " << diagnose_time_step << " " << std::endl;
	diagnostic_parameters_ << "#SIZE_X SIZE_Y" << std::endl;
	diagnostic_parameters_ << SIZE_X << " " << SIZE_Y << std::endl;

	switch ( file_for_results_->get_type() ) {
		case file_type::txt:
			break;

		case file_type::bin:
			diagnostic_parameters_ << "#sizeof(float)" << std::endl;
			diagnostic_parameters_ << sizeof(float) << std::endl;
			break;

		case file_type::hdf5:
			break;
	}
}


void density::diagnose(const Particle_parameters& sort, const std::vector<Point>& points, int t)
{
	if ((t % diagnose_time_step) == 0) {
		
		this->collect(sort, points);
		for (int y = 0; y < SIZE_Y; ++y) {
		for (int x = 0; x < SIZE_X; ++x) {
			file_for_results_->write( dens_[y*SIZE_X + x] ); 
		}
		}
		this->clear();
	}
}


void density::collect(const Particle_parameters& sort, const std::vector<Point>& points)
{
	double n  = sort.n();
	int Np = sort.Np();

	for (auto& point : points) {
		const vector2 r = point.r();

		int nx = int(roundf(r.x()/dx));
		int ny = int(roundf(r.y()/dy));

		dens_[ny*SIZE_X + nx] += n/Np;
	}
}


void density::clear()
{
	#pragma omp parallel for shared(dens_), num_threads(THREAD_NUM)
	for (int ny = 0; ny < SIZE_Y; ++ny) {
	for (int nx = 0; nx < SIZE_X; ++nx) {
		dens_[ny*SIZE_X + nx] = 0;
	}
	}
}