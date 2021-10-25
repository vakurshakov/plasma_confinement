#include "marginal_distribution.hpp" 

#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "../particles/particle/particle_parameters.hpp"
#include "../particles/particle/point.hpp"


//------- projectors ------------------------------------------------------------------------------
const double XY_projector::project_to_x(
	const Particle_parameters&_, const Point& point) const { return point.x();	}

const double XY_projector::project_to_x(
	const Particle_parameters&_, const Point& point) const { return point.y();	}


const double VxVy_projector::project_to_x(
	const Particle_parameters& sort, const Point& point) const {
		return point.px() / sqrt(sort.m()*sort.m() + point.p().dot(point.p())); }

const double VxVy_projector::project_to_y(
	const Particle_parameters& sort, const Point& point) const {
		return point.py() / sqrt(sort.m()*sort.m() + point.p().dot(point.p())); }
//-------------------------------------------------------------------------------------------------


marginal_distribution::marginal_distribution(std::string directory_path,
	/*additional*/
	double px_min, double px_max, double dpx,
	double py_min, double py_max, double dpy,
	std::unique_ptr<Projector> projector	)
	: 	Particles_diagnostic(directory_path + "/marginal_distributions/" + projector->get_coordinates())
{
	dpx_ = dpx;
	dpy_ = dpy;
	npx_min_ = px_min/dpx_;
	npx_max_ = px_max/dpx_;
	npy_min_ = py_min/dpy_;
	npy_max_ = py_max/dpy_;

	projector_ = std::move(projector);

	data_.reserve((npy_max_ - npy_min_)*(npx_max_ - npx_min_));

	this->save_parameters(directory_path);
};


void marginal_distribution::save_parameters(std::string _) 
{	
	std::ofstream diagnostic_parameters_((directory_path_ + "/parameters.txt").c_str(), std::ios::out);
	diagnostic_parameters_ << "# MARGINAL DISTRIBUTIOIN" << std::endl;
	diagnostic_parameters_ << "#TIME, dt, DTS" << std::endl;
	diagnostic_parameters_ << TIME << " " << dt << " " << diagnose_time_step << " " << std::endl;
	diagnostic_parameters_ << "#npx_min, npx_max, dpx" << std::endl;
	diagnostic_parameters_ << npx_min_ << " " << npx_max_ << " " << dpx_ << " " << std::endl;
	diagnostic_parameters_ << "#npy_min, npy_max, dpy" << std::endl;
	diagnostic_parameters_ << npy_min_ << " " << npy_max_ << " " << dpy_ << " " << std::endl;
	diagnostic_parameters_ << "#sizeof(float)" << std::endl;
	diagnostic_parameters_ << sizeof(float) << std::endl;
}


void marginal_distribution::collect(const Particle_parameters& sort, const std::vector<Point>& points)
{
	const double n  = sort.n();
	const int 	 Np = sort.Np();

	for (const auto& point : points) {
		const double pr_x = projector_->project_to_x(sort, point);
		const double pr_y = projector_->project_to_y(sort, point);
		
		const int npx = int(roundf(pr_x/dpx_));
		const int npy = int(roundf(pr_y/dpy_));

		if ((npx_min_ < npx && npx < npx_max_) &&
			(npy_min_ < npy && npy < npy_max_)) {
			data_[ (npy - npy_min_)*(npx_max_ - npx_min_) + (npx - npx_min_) ] += n/Np;
		}
	}
}


void marginal_distribution::clear()
{
	#pragma omp parallel for
	for (auto npx = 0u; npx < (npx_max_ - npx_min_); ++npx) {
	for (auto npy = 0u; npy < (npy_max_ - npy_min_); ++npy) {
		data_[ npy*(npx_max_ - npx_min_) + npx ] = 0;
	}}
}


void marginal_distribution::diagnose(const Particle_parameters& sort, const std::vector<Point>& points, int t)
{
	if ((t % diagnose_time_step) == 0) {
	
	file_for_results_ = std::make_unique<BIN_File>(directory_path_, to_string(t));

	this->collect(sort, points);

	for (auto npx = 0u; npx < (npx_max_ - npx_min_); ++npx) {
	for (auto npy = 0u; npy < (npy_max_ - npy_min_); ++npy) {
		file_for_results_->write(data_[ npy*(npx_max_ - npx_min_) + npx ]);
	}}
	
	this->clear();

	file_for_results_.release();
}}
