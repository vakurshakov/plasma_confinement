#include "./diagnostics.hpp" 


void fields_energy::initialization() {}
void particles_energy::initialization() {}


void fields_energy::diagnose(const v3f& E, const v3f& B, const v3f& j)
{
	#pragma omp parallel for reduction(+: W)
	for (int y = 0; y < SIZE_Y; ++y) {
	for (int x = 0; x < SIZE_X; ++x) {
		W += 0.5*( E.x(y,x)*E.x(y,x) + E.y(y,x)*E.y(y,x) + E.z(y,x)*E.z(y,x) +
				   B.x(y,x)*B.x(y,x) + B.y(y,x)*B.y(y,x) + B.z(y,x)*B.z(y,x) )*dx*dy;		
	}
	}
	ofs_ << W << endl;
	
	W = 0;
}

void particles_energy::diagnose(const vcp& particles)
{
	for (auto& sort : particles)
	{	
		#pragma omp parallel for reduction(+: W), num_threads(8)
		for (int i = 0; i < sort.amount(); ++i) {
			W += sqrt( sort.m()*sort.m() +
					   	sort.element(i).p().dot(sort.element(i).p()) )*
							dx*dy*sort.n()/sort.Np();
		}
	}
	ofs_ << W << "\t";

	W = 0;
}