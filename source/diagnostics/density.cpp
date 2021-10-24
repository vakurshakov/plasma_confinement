#include "density.hpp"

#include <string>
#include <vector>
#include <memory>

#include "../particles/particle/particle_parameters.hpp"
#include "../particles/particle/point.hpp"
#include "../constants.h"


density::density(std::string directory_path,
	int begin_x=0, int begin_y=0, int end_x=SIZE_X, int end_y=SIZE_Y)
	: Particles_diagnostic(directory_path)
{
	begin_x_ = begin_x;
	begin_y_ = begin_y;
	end_x_ 	 = end_x;
	end_y_ 	 = end_y;

	dens_.reserve(SIZE_X*SIZE_Y);
	this->clear();
	
	this->save_parameters(directory_path);
}

void density::save_parameters(std::string directory_path)
{
	std::ofstream diagnostic_parameters_((directory_path + "/parameters.txt").c_str(), std::ios::out);
	diagnostic_parameters_ << "#TIME dt DTS" << std::endl;
	diagnostic_parameters_ << TIME << " " << dt << " " << diagnose_time_step << " " << std::endl;
	diagnostic_parameters_ << "#begin_x, end_x, dx" << std::endl;
	diagnostic_parameters_ << begin_x_ << " " << end_x_ << " " << dx << std::endl;
	diagnostic_parameters_ << "#begin_y, end_y, dy" << std::endl;
	diagnostic_parameters_ << begin_y_ << " " << end_y_ << " " << dy << std::endl;
	diagnostic_parameters_ << "#sizeof(float)" << std::endl;
	diagnostic_parameters_ << sizeof(float) << std::endl;
}


void density::diagnose(const Particle_parameters& sort, const std::vector<Point>& points, int t)
{
	if ((t % diagnose_time_step) == 0) {

	file_for_results_ = std::make_unique<BIN_File>(directory_path_, to_string(t));

	this->collect(sort, points);

	for (int y = 0; y < SIZE_Y; ++y) {
	for (int x = 0; x < SIZE_X; ++x) {
		file_for_results_->write( dens_[y*SIZE_X + x] ); 
	}}

	this->clear();

	file_for_results_.release();
}}


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
	}}
}