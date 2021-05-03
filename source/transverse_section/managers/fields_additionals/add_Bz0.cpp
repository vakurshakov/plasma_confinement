#include "../fields.hpp"
#include <cmath>


void Fields::add_Bz0(double Bz0)
{
	#pragma omp parallel for shared(B_), num_threads(THREAD_NUM)
	for (int y = (*B_).begin_y(X); y < (*B_).end_y(X); ++y) {
	for (int x = (*B_).begin_x(X); x < (*B_).end_x(X); ++x) {
		(*B_).x(y,x) += Bz0;
	}
	}
}
