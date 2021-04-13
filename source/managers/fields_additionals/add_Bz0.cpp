#include "../fields.hpp"
#include <cmath>


void Fields::add_Bz0(double Bz0)
{
	#pragma omp parallel for shared(B_), num_threads(THREAD_NUM)
	for (int y = (*B_).begin_y(Z); y < (*B_).end_y(Z); ++y) {
	for (int x = (*B_).begin_x(Z); x < (*B_).end_x(Z); ++x) {
		(*B_).z(y,x) += Bz0;
	}
	}
}
