#include "distribution_moment.hpp" 

#include <cmath>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>

#include "../particles/particle/concrete/particle_interface.hpp"
#include "../particles/particle/parameters/global_parameters.hpp"

namespace fs = std::filesystem;


//------- additive --------------------------------------------------------------------------------
const vector3 get_velocity(const IParticle* const particle)
{
	const double m = particle->m();
	const vector3& p = particle->get_point().p();

	return p / sqrt(m * m + p.dot(p));
}


//------- projectors ------------------------------------------------------------------------------
const double XY_projector::project_to_x(const IParticle* const particle) const
{
	return particle->get_point().x();
}

const double XY_projector::project_to_y(const IParticle* const particle) const
{
	return particle->get_point().y();
}


const double VxVy_projector::project_to_x(const IParticle* const particle) const
{
	return get_velocity(particle).x();
}

const double VxVy_projector::project_to_y(const IParticle* const particle) const
{
	return get_velocity(particle).y();
}


//------- moment ----------------------------------------------------------------------------------
const double zeroth_moment::get_quantity_to_be_averaged_(const IParticle* const) const
{
	return 1.;
}


const double first_Vx_moment::get_quantity_to_be_averaged_(const IParticle* const particle) const
{
	return get_velocity(particle).x();
}


const double first_Vy_moment::get_quantity_to_be_averaged_(const IParticle* const particle) const
{
	return get_velocity(particle).y();
}


distribution_moment::distribution_moment(std::string directory_path,
	/*additional*/
	double px_min, double px_max, double dpx,
	double py_min, double py_max, double dpy,
	std::unique_ptr<Projector> projector,
	std::unique_ptr<Moment>    moment	)
	: 	Particles_diagnostic(
			directory_path + "/" +
			moment->get_moment_name() + "_of_" +
			projector->get_axes_names())
	{
		dpx_ = dpx;
		dpy_ = dpy;
		npx_min_ = int(round(px_min / dpx_));
		npy_min_ = int(round(py_min / dpy_));
		npx_max_ = int(round(px_max / dpx_));
		npy_max_ = int(round(py_max / dpy_));

		projector_ = std::move(projector);
		moment_	   = std::move(moment);

		data_.reserve((npx_max_ - npx_min_) * (npy_max_ - npy_min_));

		this->save_parameters(directory_path_);
	}


void distribution_moment::save_parameters(std::string directory_path) 
{	
	fs::create_directories(fs::path(directory_path));
	std::ofstream diagnostic_parameters_((directory_path + "/parameters.txt").c_str(), std::ios::out);
	diagnostic_parameters_ << "#TIME, dt, DTS" << std::endl;
	diagnostic_parameters_ << TIME << " " << dt << " " << diagnose_time_step << " " << std::endl;
	diagnostic_parameters_ << "#npx_min, npx_max, dpx" << std::endl;
	diagnostic_parameters_ << npx_min_ << " " << npx_max_ << " " << dpx_ << " " << std::endl;
	diagnostic_parameters_ << "#npy_min, npy_max, dpy" << std::endl;
	diagnostic_parameters_ << npy_min_ << " " << npy_max_ << " " << dpy_ << " " << std::endl;
	diagnostic_parameters_ << "#sizeof(float), CHAR_BIT" << std::endl;
	diagnostic_parameters_ << sizeof(float) << " " << __CHAR_BIT__ << " " << std::endl;
}


//! @todo interpolated particle density.
void distribution_moment::collect(
	const gParameters& parameters, const std::vector<particle_up>& particles)
	{
		const int Np = parameters.Np();

		for (const auto& up_particle : particles) {
			const double pr_x = projector_->project_to_x(up_particle.get());
			const double pr_y = projector_->project_to_y(up_particle.get());

			const int npx = int(round(pr_x / dpx_));   
			const int npy = int(round(pr_y / dpy_));

			if ((npx_min_ < npx && npx < npx_max_) &&
				(npy_min_ < npy && npy < npy_max_))
			{
				const double n = up_particle->n();

				data_[(npy - npy_min_) * (npx_max_ - npx_min_) +  (npx - npx_min_)]
					+= moment_->get_quantity_to_be_averaged_(up_particle.get()) * n/Np;
			}
			else
				continue;
		}
	}


void distribution_moment::clear()
{
	#pragma omp parallel for
	for (auto npy = 0; npy < (npx_max_ - npx_min_); ++npy) {
	for (auto npx = 0; npx < (npy_max_ - npy_min_); ++npx) {
		data_[ npy * (npx_max_ - npx_min_) + npx ] = 0;
	}}
}


void distribution_moment::diagnose(
	const gParameters& parameters, const std::vector<particle_up>& particles, int t)
	{
		if ((t % diagnose_time_step) == 0) {
		
		file_for_results_ = std::make_unique<BIN_File>(directory_path_, to_string(t));

		this->collect(parameters, particles);

		for (auto npy = 0; npy < (npx_max_ - npx_min_); ++npy) {
		for (auto npx = 0; npx < (npy_max_ - npy_min_); ++npx) {
			file_for_results_->write(data_[ npy * (npx_max_ - npx_min_) + npx ]);
		}}

		this->clear();
	}}
