#include "set_Bz_distribution.hpp"

Set_Bz_distribution::Set_Bz_distribution(Fields* const fields) : fields_(fields) {}

void Set_Bz_distribution::execute(int /* timestep */) const {
  PROFILE_FUNCTION();

  for (int y = fields_->B().iy_first(Z); y < fields_->B().iy_last(Z); ++y) {
#if PLASMA_MAG_FIELD_SETUP
  for (int x = SIZE_X / 2; x < fields_->B().ix_last(Z); ++x) {
#else
  for (int x = fields_->B().ix_first(Z); x < fields_->B().ix_last(Z); ++x) {
#endif

    fields_->B().z(y, x) = config::Omega_max;    
  }}
}