#include "phase_diagram.hpp" 

#include <cmath>
#include <string>
#include <vector>

#include "../particles/particle/particle_parameters.hpp"
#include "../particles/particle/point.hpp"


diagram_vx_on_y::diagram_vx_on_y(std::string directory_path, double dv)
	: 	Particles_diagnostic(directory_path + "/" + "diagram_vx_on_y"), dv_(dv)
	{
		nv_min_ = v_min_/dv_;
		nv_max_ = v_max_/dv_;
		data_.reserve((ny_max_ - ny_min_)*(nv_max_ - nv_min_));

		this->save_parameters(directory_path);
	};


void diagram_vx_on_y::save_parameters(std::string directory_path)
{	
	std::ofstream diagnostic_parameters_((directory_path + "/parameters.txt").c_str(), std::ios::out);
	diagnostic_parameters_ << "#TIME, dt, DTS" << std::endl;
	diagnostic_parameters_ << TIME << " " << dt << " " << diagnose_time_step << " " << std::endl;
	diagnostic_parameters_ << "#nymin =  0, nymax = SIZE_Y, dy" << std::endl;
	diagnostic_parameters_ << ny_min_ << " " << ny_max_ << " " << dy << " " << std::endl;
	diagnostic_parameters_ << "#nv_min = -c/dv, nvmax = +c/dv, dv" << std::endl;
	diagnostic_parameters_ << nv_min_ << " " << nv_max_ << " " << dv_ << " " << std::endl;
	diagnostic_parameters_ << "#sizeof(float)" << std::endl;
	diagnostic_parameters_ << sizeof(float) << std::endl;
}


void diagram_vx_on_y::collect_diagram_vx_on_y(const Particle_parameters& sort, const std::vector<Point>& points)
{
	double n  = sort.n();
	int Np = sort.Np();

	for (const auto& point : points) {
		double y  = point.y();
		double vx = point.px() / sqrt(sort.m()*sort.m() + point.p().dot(point.p()));

		int ny  = int(roundf(y/dy ));
		int nvx = int(roundf(vx/dv_));

		if ((ny_min_ < ny && ny  < ny_max_) && (nv_min_ < nvx && nvx < nv_max_)) {
			data_[ (nvx - nv_min_)*(ny_max_ - ny_min_) + (ny - ny_min_) ] += n/Np;
		}
	}
}


void diagram_vx_on_y::clear_diagram_vx_on_y()
{
	#pragma omp parallel for num_threads(THREAD_NUM)
	for (int nvx = 0; nvx < (nv_max_ - nv_min_); ++nvx) {
	for (int ny = ny_min_;  ny < ny_max_;  ++ny) {
		data_[ nvx*(ny_max_ - ny_min_) + (ny - ny_min_) ] = 0;
	}}
}


void diagram_vx_on_y::diagnose(const Particle_parameters& sort, const std::vector<Point>& points, int t)
{
	if ((t % diagnose_time_step) == 0) {
	
	file_for_results_ = std::make_unique<BIN_File>(directory_path_, to_string(t));

	this->collect_diagram_vx_on_y(sort, points);

	for (int nvx = 0; nvx < (nv_max_ - nv_min_); ++nvx) {
	for (int ny = ny_min_;  ny < ny_max_; ++ ny) {
		file_for_results_->write(data_[ nvx*(ny_max_ - ny_min_) + ny ]);
	}}
	
	this->clear_diagram_vx_on_y();

	file_for_results_.release();
}}
