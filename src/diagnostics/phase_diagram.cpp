#include "./phase_diagram.hpp" 

#include <cmath>
#include <string>
#include <vector>

#include "../particle/particle_parameters.hpp"
#include "../particle/point.hpp"
#include "../constants.h"


void collect_diagram_vx_on_y(diagram_vx_on_y& diag,
	const Particle_parameters& sort, const std::vector<Point>& points)
{
	/*
	double n  = sort.n();
	int Np = sort.Np();

	for (int i = 0; i < sort.amount(); ++i) {
		Particle i_th = sort.element(i); 

		double ry = i_th.y();
		double vx = i_th.px() / 
			sqrt(sort.m()*sort.m() + i_th.p().dot(i_th.p()));

		int nY  = int(roundf(ry/dy ));
		int nVx = int(roundf(vx/diag.dv_));

		diag.data_[ (nVx - diag.nVmin_)*(diag.nYmax_ - diag.nYmin_) + (nY - diag.nYmin_) ] += n/Np;
	}
	*/
}

void clear_diagram_vx_on_y(diagram_vx_on_y& diag)
{
	/*
	#pragma omp parallel for num_threads(THREAD_NUM)
	for (int nVx = 0; nVx < (diag.nVmax_ - diag.nVmin_); ++nVx) {
	for (int  nY = 0;  nY < (diag.nYmax_ - diag.nYmin_); ++ nY) {
		diag.data_[ nVx*(diag.nYmax_ - diag.nYmin_) + nY ] = 0;
	}
	}
	*/
}


void diagram_vx_on_y::save_parameters(std::string directory_path)
{	
	/*
	std::ofstream diagnostic_parameters_((path_to_it + "/parameters.txt").c_str(), ios::out);
	diagnostic_parameters_ << "#TIME dt" << std::endl;
	diagnostic_parameters_ << TIME << " " << dt << " " << std::endl;
	diagnostic_parameters_ << "#SIZE_X SIZE_Y DTS" << std::endl;
	diagnostic_parameters_ << SIZE_X << " " << SIZE_Y << " " << diagnose_time_step << std::endl;
	

	ofs_ << "#DTS" << std::endl;
	ofs_ << diagnose_time_step << std::endl;
	ofs_ << "#nVmin_ nVmax_ dv_" << std::endl;
	ofs_ << nVmin_ << " " << nVmax_ << " " << dv_ << std::endl;
	ofs_ << "#nYmin_ nYmax_ dy"  << std::endl;
	ofs_ << nYmin_ << " " << nYmax_ << " " << dy  << std::endl;
	data_.reserve((nYmax_ - nYmin_)*(nVmax_ - nYmin_));
	clear_diagram_vx_on_y(*this);
	*/
}


void diagram_vx_on_y::diagnose(const Particle_parameters& sort, const std::vector<Point>& points, int t)
{
	/*
	if ((t % diagnose_time_step) == 0) {
			
		collect_diagram_vx_on_y(*this, sort);
	
		for (int nVx = 0; nVx < (nVmax_ - nVmin_); ++nVx) {
		for (int  nY = 0;  nY < (nYmax_ - nYmin_); ++ nY) {
			OFS_ << data_[ nVx*(nYmax_ - nYmin_) + nY ] << " ";
		}
			OFS_ << "\t";
		}
		OFS_ << std::endl;
	
		clear_diagram_vx_on_y(*this);
	}
	*/
}
