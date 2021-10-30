#include "energy.hpp" 

#include <omp.h>
#include <string>
#include <memory>
#include <filesystem>

#include "../particles/particle/particle_parameters.hpp"
#include "../particles/particle/point.hpp"
#include "../vectors/vector3_field.hpp"
#include "../constants.h"

using v3f = vector3_field;
namespace fs = std::filesystem;


//######## fields energy ##########################################################################


fields_energy::fields_energy(std::string directory_path)
	: Fields_diagnostic(directory_path + "fields_energy")
	{
		this->save_parameters(directory_path);
		file_for_results_ = std::make_unique<BIN_File>(directory_path_, "total");
	};

void fields_energy::save_parameters(std::string directory_path)
{
	fs::create_directories(fs::path(directory_path));
	std::ofstream diagnostic_parameters_((directory_path + "/parameters.txt").c_str(), std::ios::out);
	diagnostic_parameters_ << "#TIME dt DTS" << std::endl;
	diagnostic_parameters_ << TIME << " " << dt << " " << diagnose_time_step << " " << std::endl;
	diagnostic_parameters_ << "#sizeof(float)" << std::endl;
	diagnostic_parameters_ << sizeof(float) << std::endl;
}


void fields_energy::diagnose(const v3f& E, const v3f& B, const v3f& j, int t)
{
	#pragma omp parallel for reduction(+: W)
	for (int y = 0; y < SIZE_Y; ++y) {
	for (int x = 0; x < SIZE_X; ++x) {
		W += 0.5*( E.x(y,x)*E.x(y,x) + E.y(y,x)*E.y(y,x) + E.z(y,x)*E.z(y,x) +
				   B.x(y,x)*B.x(y,x) + B.y(y,x)*B.y(y,x) + B.z(y,x)*B.z(y,x) )*dx*dy;		
	}
	}
	file_for_results_->write(W);
	W = 0;
}

//######## particles energy #######################################################################


particles_energy::particles_energy(std::string directory_path)
	: Particles_diagnostic(directory_path)
	{
		this->save_parameters(directory_path);
		file_for_results_ = std::make_unique<BIN_File>(directory_path_, "total");
	};


// как убрать дублирование кода в этом случае?
void particles_energy::save_parameters(std::string directory_path)
{
	std::ofstream diagnostic_parameters_((directory_path + "/parameters.txt").c_str(), std::ios::out);
	diagnostic_parameters_ << "#TIME dt DTS" << std::endl;
	diagnostic_parameters_ << TIME << " " << dt << " " << diagnose_time_step << " " << std::endl;
	diagnostic_parameters_ << "#sizeof(float)" << std::endl;
	diagnostic_parameters_ << sizeof(float) << std::endl;
}


void particles_energy::diagnose(const Particle_parameters& sort, const std::vector<Point>& points, int t)
{
	#pragma omp parallel for reduction(+: W)
	for (decltype(points.size()) i = 0; i < points.size(); ++i) {
		W += sqrt( sort.m()*sort.m() + points[i].p().dot(points[i].p()) )*
				dx*dy*sort.n()/sort.Np();
	}
	file_for_results_->write(W);
	W = 0;
}