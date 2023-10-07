#include "./fields.hpp"
#include <cmath>


void Fields::add_Bz0(double Bz0)
{
	for (int ny = B_->y_min(Z); ny < B_->y_max(Z); ++ny) {
	for (int nx = B_->x_min(Z); nx < B_->x_max(Z); ++nx) {
		B_->z(ny,nx) += Bz0;
	}}
}
