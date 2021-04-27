#include "./diagnostics.hpp" 


void phase_diagram::initialize()
{
	ofs_ << vmin_ << " " << vmax_ << std::endl;
	ofs_ << xmin_ << " " << xmax_ << std::endl;
}

void phase_diagram::diagnose(const Species_description& sort)
{
	double phase_dens[ int((vmax_ - vmin_)/dv_)*(xmax_ - xmin_) ];


	for (double vx = vmin_; vx < vmax_-dv_; vx += dv_) {
	#pragma omp parllel for
	for (int  x = xmin_;  x < xmax_-1; ++ x) {
		phase_dens[ int((vx - vmin_)/dv_)*(xmax_ - xmin_) + (x - xmin_) ] = 0;
	}}


	for (double vx = vmin_; vx < vmax_; vx += dv_) {
	for (int  x = xmin_; x < xmax_; ++ x) {
				
		double vx_i;
		
		#pragma omp parllel for
		for (int i = 0; i < sort.amount(); ++i) {
			
			vx_i = sort.element(i).px() / sqrt(sort.m()*sort.m() + sort.element(i).px()*sort.element(i).px());

			if (( x*dx  <= sort.element(i).x()  && sort.element(i).x()  < (x + 1)*dx ) && 
				( vx <= vx_i && vx_i < vx + dv_ ))
			{
				#pragma atomic
				phase_dens[ int((vx - vmin_)/dv_)*(xmax_ - xmin_) + (x - xmin_) ] += sort.n(); 
			}

		}
		ofs_ << phase_dens[ int((vx - vmin_)/dv_)*(xmax_ - xmin_) + (x - xmin_) ] << " ";
	}
		ofs_ << "\t";	
	}
}
