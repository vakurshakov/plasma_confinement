#include "vector3_field.hpp"

#pragma omp declare simd linear(g: 1), uniform(begin, end), notinbranch
inline int periodic(int g, int begin, int end) {
  int pg = g % (end - begin);

  if (g < begin)
    pg += (end - begin);

  return pg;
}

#pragma omp declare simd linear(g: 1), uniform(begin, end), notinbranch
inline int continuous(int g, int begin, int end) {
  if (begin <= g && g < end) {
    return g;
  }
  else if (g < begin) {
    return begin;
  }
  else {
    return end - 1;
  }
}

#pragma omp declare simd linear(g: 1), uniform(begin, end), notinbranch
inline bool in_bounds(int g, int begin, int end) {
  return begin <= g && g < end;
}

#define CAT_INNER(A, B) A ## B
#define CAT(A, B) CAT_INNER(A, B)

#define REFLECTIVE_CALL(DIR, AXIS)                                                     \
  if (!in_bounds(n##DIR, bound_.CAT(DIR, _min[AXIS]), bound_.CAT(DIR, _max[AXIS]))) {  \
    zero_ = 0.0;                                                                       \
    return zero_;                                                                      \
  }                                                                                    \

#if (Y_BOUNDARY_CONDITION == PEC || Y_BOUNDARY_CONDITION == PMC)
# define PROCESS_Y(AXIS) REFLECTIVE_CALL(y, AXIS)
#elif (Y_BOUNDARY_CONDITION == PERIODIC)
# define PROCESS_Y(AXIS) ny = periodic(ny, 0, size_y_)
#elif (Y_BOUNDARY_CONDITION == CONTINUOUS)
# define PROCESS_Y(AXIS) ny = continuous(ny, 0, size_y_)
#elif (Y_BOUNDARY_CONDITION != NONE)
# error "Unkown Y_BOUNDARY_CONDITION is specified!"
#endif

#if (X_BOUNDARY_CONDITION == PEC || X_BOUNDARY_CONDITION == PMC)
# define PROCESS_X(AXIS) REFLECTIVE_CALL(x, AXIS)
#elif (X_BOUNDARY_CONDITION == PERIODIC)
# define PROCESS_X(AXIS) nx = periodic(nx, 0, size_x_)
#elif (X_BOUNDARY_CONDITION == CONTINUOUS)
# define PROCESS_X(AXIS) nx = continuous(nx, 0, size_x_)
#elif (X_BOUNDARY_CONDITION != NONE)
# error "Unkown X_BOUNDARY_CONDITION is specified!"
#endif

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

constexpr int vector3_field::index(int ny, int nx) const {
  return ny * size_x_ + nx;
}

#define UPPER_x X
#define UPPER_y Y
#define UPPER_z Z

#define PROCESS(COMPONENT)                    \
  PROCESS_Y(UPPER_##COMPONENT);               \
  PROCESS_X(UPPER_##COMPONENT);               \
  return CAT(f##COMPONENT, _)[index(ny, nx)]  \

double& vector3_field::x(int ny, int nx) { PROCESS(x); }
double& vector3_field::y(int ny, int nx) { PROCESS(y); }
double& vector3_field::z(int ny, int nx) { PROCESS(z); }

const double& vector3_field::x(int ny, int nx) const { PROCESS(x); }
const double& vector3_field::y(int ny, int nx) const { PROCESS(y); }
const double& vector3_field::z(int ny, int nx) const { PROCESS(z); }

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
