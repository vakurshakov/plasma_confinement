#include "vector3_field.hpp"

vector3_field::vector3_field(int size_x, int size_y) {
  size_x_ = size_x;
  size_y_ = size_y;

  fx_.reserve(size_x_ * size_y_);
  fy_.reserve(size_x_ * size_y_);
  fz_.reserve(size_x_ * size_y_);

  bound_ = {
    .x_min = {0, 0, 0},
    .y_min = {0, 0, 0},
    .x_max = {size_x_, size_x_, size_x_},
    .y_max = {size_y_, size_y_, size_y_},
  };
}

vector3_field::vector3_field(const std::string& type, int size_x, int size_y)
    : vector3_field(size_x, size_y) {
#if (X_BOUNDARY_CONDITION == PEC || X_BOUNDARY_CONDITION == PMC)
  if ((X_BOUNDARY_CONDITION == PEC && type == "Electric") ||
      (X_BOUNDARY_CONDITION == PMC && type == "Magnetic")) {
    bound_.x_min[X] = 0;  bound_.x_max[X] = size_x_;
    bound_.x_min[Y] = 1;  bound_.x_max[Y] = size_x_;
    bound_.x_min[Z] = 0;  bound_.x_max[Z] = size_x_ - 1;
  }
  if ((X_BOUNDARY_CONDITION == PEC && type == "Magnetic") ||
      (X_BOUNDARY_CONDITION == PMC && type == "Electric")) {
    bound_.x_min[X] = 0;  bound_.x_max[X] = size_x_ - 1;
    bound_.x_min[Y] = 0;  bound_.x_max[Y] = size_x_;
    bound_.x_min[Z] = 0;  bound_.x_max[Z] = size_x_;
  }
#endif

#if (Y_BOUNDARY_CONDITION == PEC || Y_BOUNDARY_CONDITION == PMC)
  if ((Y_BOUNDARY_CONDITION == PEC && type == "Electric") ||
      (Y_BOUNDARY_CONDITION == PMC && type == "Magnetic")) {
		bound_.y_min[X] = 1;  bound_.y_max[X] = size_y_;
    bound_.y_min[Y] = 0;  bound_.y_max[Y] = size_y_;
    bound_.y_min[Z] = 0;  bound_.y_max[Z] = size_y_ - 1;
	}
	if ((Y_BOUNDARY_CONDITION == PEC && type == "Magnetic") ||
      (Y_BOUNDARY_CONDITION == PMC && type == "Electric")) {
		bound_.y_min[X] = 0;  bound_.y_max[X] = size_y_;
    bound_.y_min[Y] = 0;  bound_.y_max[Y] = size_y_ - 1;
    bound_.y_min[Z] = 0;  bound_.y_max[Z] = size_y_;
	}
#endif
}

double& vector3_field::operator()(Axis comp, int ny, int nx) {
  return const_cast<double&>(std::as_const(*this).operator()(comp, ny, nx));
}

const double& vector3_field::operator()(Axis comp, int ny, int nx) const {
  switch (comp) {
    case Axis::X: return x(ny, nx);
    case Axis::Y: return y(ny, nx);
    case Axis::Z: return z(ny, nx);
  }
  zero_ = 0.0;
  return zero_;
}

vector3 vector3_field::operator()(int ny, int nx) const {
  return {
    x(ny, nx),
    y(ny, nx),
    z(ny, nx),
  };
}
