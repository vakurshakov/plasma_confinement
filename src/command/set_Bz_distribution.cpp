#include "set_Bz_distribution.hpp"

Set_Bz_distribution::Set_Bz_distribution(Fields* const fields) : fields_(fields) {}

void Set_Bz_distribution::execute(int /* timestep */) const {
  PROFILE_FUNCTION();

  static auto __Bz = Table_function("src/utils/transition_layer/Bz_" + config::postfix);

  for (int y = fields_->B().iy_first(Z); y < fields_->B().iy_last(Z); ++y) {
  for (int x = fields_->B().ix_first(Z); x < fields_->B().ix_last(Z); ++x) {

#if !BEAM_INJECTION_SETUP
    if (x * dx <= __Bz.get_x0()) {
      fields_->B().z(y, x) = 0.0;
    }
    else if (x * dx <= __Bz.get_xmax()) {
      fields_->B().z(y, x) = __Bz(x * dx);
    }
    else {
      fields_->B().z(y, x) = __Bz(__Bz.get_xmax());
    }
#else
    fields_->B().z(y, x) = config::Omega_max;
#endif
  }}
}
