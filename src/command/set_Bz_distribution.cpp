#include "set_Bz_distribution.hpp"

#define M_SQRT_PI 1.77245385090551602730

Set_Bz_distribution::Set_Bz_distribution(Fields* const fields) : fields_(fields) {}

static auto& __func = transition_layer::Parameter_function::instance();

void Set_Bz_distribution::execute(int /* timestep */) const {
  PROFILE_FUNCTION();

  for (int y = fields_->B().iy_first(Z); y < fields_->B().iy_last(Z); ++y) {
  for (int x = fields_->B().ix_first(Z); x < fields_->B().ix_last(Z); ++x) {
    if (x * dx < __func.get_x0()) {
      continue;
    }
    else if (x * dx <= __func.get_xmax()) {
      double fx = __func.get_value(x * dx);
      
      fields_->B().z(y, x) = config::Omega_max / M_SQRT_PI *
        sqrt(M_PI / 2. - asin(fx) - fx * sqrt(1 - fx * fx));
    }
    else { // x * dx >= __func.get_xmax()
      fields_->B().z(y, x) = config::Omega_max;
    }
  }}
}