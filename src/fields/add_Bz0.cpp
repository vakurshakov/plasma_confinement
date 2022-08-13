#include "./fields.hpp"
#include <cmath>


void Fields::add_Bz0(double Bz0)
{
	for (int ny = B_->iy_first(Z); ny < B_->iy_last(Z); ++ny) {
	for (int nx = B_->ix_first(Z); nx < B_->ix_last(Z); ++nx) {
		B_->z(ny,nx) += Bz0;
	}
	}
}
