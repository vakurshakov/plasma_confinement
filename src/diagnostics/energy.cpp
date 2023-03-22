#include "energy.hpp"
#include "src/file_writers/bin_file.hpp"

#if !BEAM_INJECTION_SETUP

#include "src/utils/transition_layer/table_function.hpp"
static auto __Bz = Table_function("src/utils/transition_layer/Bz_" + config::postfix);

#endif

namespace fs = std::filesystem;

energy_parameters_saver::energy_parameters_saver(
    std::string result_directory)
    : Diagnostic(result_directory) {
  save_parameters();
}

void energy_parameters_saver::save_parameters() const {}


fields_energy::fields_energy(
    std::string result_directory,
    const vector3_field& electric,
    const vector3_field& magnetic)
    : energy_parameters_saver(result_directory),
      electric_(electric),
      magnetic_(magnetic) {
#if !START_FROM_BACKUP
  file_for_results_ = std::make_unique<BIN_File>(
    result_directory_, "fields_energy");

#else
  file_for_results_ = std::make_unique<BIN_File>(
    BIN_File::from_backup(result_directory_, "fields_energy"));

#endif
}

void fields_energy::diagnose(int t) {
  PROFILE_FUNCTION();

  double WEx = 0, WEy = 0, WEz = 0;
  double WBx = 0, WBy = 0, WBz = 0, WdBz = 0;

  #pragma omp parallel for reduction(+: WEx, WEy, WEz, WBx, WBy, WBz, WdBz)
  for (int y = 0; y < SIZE_Y; ++y) {
  for (int x = 0; x < SIZE_X; ++x) {
    WEx += 0.5 * electric_.x(y, x) * electric_.x(y, x) * dx * dy;
    WEy += 0.5 * electric_.y(y, x) * electric_.y(y, x) * dx * dy;
    WEz += 0.5 * electric_.z(y, x) * electric_.z(y, x) * dx * dy;

    WBx += 0.5 * magnetic_.x(y, x) * magnetic_.x(y, x) * dx * dy;
    WBy += 0.5 * magnetic_.y(y, x) * magnetic_.y(y, x) * dx * dy;
    WBz += 0.5 * magnetic_.z(y, x) * magnetic_.z(y, x) * dx * dy;

#if BEAM_INJECTION_SETUP
    WdBz += 0.5 * (magnetic_.z(y, x) - config::Omega_max) * (magnetic_.z(y, x) - config::Omega_max) * dx * dy;
#else
    double B0 = 0.0;

    if (x * dx < __Bz.get_x0()) {
      B0 = 0.0;
    }
    else if (x * dx < __Bz.get_xmax()) {
      B0 = __Bz(x * dx);
    }
    else {
      B0 = __Bz(__Bz.get_xmax());
    }

    WdBz += 0.5 * (magnetic_.z(y, x) - B0) * (magnetic_.z(y, x) - B0) * dx * dy;
#endif
  }}

  file_for_results_->write(WEx);
  file_for_results_->write(WEy);
  file_for_results_->write(WEz);
  file_for_results_->write(WBx);
  file_for_results_->write(WBy);
  file_for_results_->write(WBz);
  file_for_results_->write(WdBz);

  file_for_results_->write(WEx + WEy + WEz + WBx + WBy + WBz);

  LOG_INFO("Fields energy: Ex = {:.5e}, Ey = {:.5e}, Ez = {:.5e}", WEx, WEy, WEz);
  LOG_INFO("               Bx = {:.5e}, By = {:.5e}, Bz = {:.5e}", WBx, WBy, WBz);
  LOG_INFO("            Total = {}", WEx + WEy + WEz + WBx + WBy + WBz);

  if (t % diagnose_time_step == 0)
    file_for_results_->flush();
}


particles_energy::particles_energy(
    std::string result_directory, std::string sort_name,
    const Particles& particles)
    : energy_parameters_saver(result_directory),
      particles_(particles) {
#if !START_FROM_BACKUP
  file_for_results_ = std::make_unique<BIN_File>(
    result_directory_, sort_name + "_energy");

#else
  file_for_results_ = std::make_unique<BIN_File>(
    BIN_File::from_backup(result_directory_, sort_name + "_energy"));

#endif
}

void particles_energy::diagnose(int t) {
  PROFILE_FUNCTION();

  const int Np = particles_.get_parameters().Np();

  double W = 0;

  #pragma omp parallel for reduction(+: W)
  for (const auto& particle : particles_.get_particles()) {
    const double m = particle.m();
    const double n = particle.n();
    const vector3& p = particle.point.p;

    W += (sqrt(m * m + p.square()) - m) * dx * dy * n / Np;
  }

  file_for_results_->write(W);

  LOG_INFO("Energy in {} = {}", particles_.get_parameters().get_name(), W);

  if (t % diagnose_time_step == 0)
    file_for_results_->flush();
}
