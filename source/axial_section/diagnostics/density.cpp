#include "./diagnostics.hpp" 

void collect_density(vector<double>& dens, const Species_description& sort)
{
	double n  = sort.n();
	int Np = sort.Np();

	for (int i = 0; i < sort.amount(); ++i) {
		vector2 r = sort.element(i).r();

		int nx = int(roundf(r.x/dx));
		int ny = int(roundf(r.y/dy));

		dens[ny*SIZE_X + nx] += n/Np;
	}
}


void clear_density(vector<double>& dens)
{
	#pragma omp parallel for num_threads(THREAD_NUM)
	for (int y = 0; y < SIZE_Y; ++y) {
	for (int x = 0; x < SIZE_X; ++x) {
		dens[y*SIZE_X + x] = 0;
	}
	}
}


void density::initialize()
{
	ofs_ << "#SIZE_X SIZE_Y" << endl;
	ofs_ << SIZE_X << " " << SIZE_Y << " " << endl;
	dens_.reserve(SIZE_X*SIZE_Y);
	clear_density(dens_);
}


void density::diagnose(const Species_description& sort)
{
	collect_density(dens_, sort);
	
	float temp;
	for (int y = 0; y < SIZE_Y; ++y) {
	for (int x = 0; x < SIZE_X; ++x) {
		temp = dens_[y*SIZE_X + x];
		OFS_.write( (char*)&temp, sizeof(float) ); 
	}
	}

	clear_density(dens_);
}
