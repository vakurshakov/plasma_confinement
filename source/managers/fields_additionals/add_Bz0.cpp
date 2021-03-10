#include "../fields.hpp"
#include <cmath>


void Fields::add_Bz0(double Bz0)
{
	#pragma omp parallel for shared(B_), num_threads(8)
	for (int y = 0; y < (*B_).size_y(); ++y) {
	for (int x = 0; x < (*B_).size_x(); ++x) {
		(*B_).z(y,x) += Bz0;
	}
	}
}
