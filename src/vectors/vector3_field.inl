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

#define REFLECTIVE_CALL(DIR, AXIS)                                       \
  if (!in_bounds(n##DIR, CAT(DIR, _min(AXIS)), CAT(DIR, _max(AXIS)))) {  \
    zero_ = 0.0;                                                         \
    return zero_;                                                        \
  }                                                                      \

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

inline double& vector3_field::x(int ny, int nx) { PROCESS(x); }
inline double& vector3_field::y(int ny, int nx) { PROCESS(y); }
inline double& vector3_field::z(int ny, int nx) { PROCESS(z); }

inline const double& vector3_field::x(int ny, int nx) const { PROCESS(x); }
inline const double& vector3_field::y(int ny, int nx) const { PROCESS(y); }
inline const double& vector3_field::z(int ny, int nx) const { PROCESS(z); }
