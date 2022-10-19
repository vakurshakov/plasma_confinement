#include "energy.hpp"
#include "src/file_writers/bin_file.hpp"

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
  file_for_results_ = std::make_unique<BIN_File>(
    result_directory_, "fields_energy");
}

void fields_energy::diagnose(int t) {
  PROFILE_FUNCTION();

  double Wex = 0, Wey = 0, Wez = 0;
  double Wbx = 0, Wby = 0, Wbz = 0;

  #pragma omp parallel for reduction(+: Wex, Wey, Wez, Wbx, Wby, Wbz)
  for (int y = 0; y < SIZE_Y; ++y) {
  for (int x = 0; x < SIZE_X; ++x) {
    Wex += 0.5 * electric_.x(y, x) * electric_.x(y, x) * dx * dy;
    Wey += 0.5 * electric_.y(y, x) * electric_.y(y, x) * dx * dy;
    Wez += 0.5 * electric_.z(y, x) * electric_.z(y, x) * dx * dy;

    Wbx += 0.5 * magnetic_.x(y, x) * magnetic_.x(y, x) * dx * dy;
    Wby += 0.5 * magnetic_.y(y, x) * magnetic_.y(y, x) * dx * dy;
    Wbz += 0.5 * magnetic_.z(y, x) * magnetic_.z(y, x) * dx * dy;
  }}

  file_for_results_->write(Wex);
  file_for_results_->write(Wey);
  file_for_results_->write(Wez);
  file_for_results_->write(Wbx);
  file_for_results_->write(Wby);
  file_for_results_->write(Wbz);

  file_for_results_->write(Wex + Wey + Wez + Wbx + Wby + Wbz);

  if (t % diagnose_time_step == 0)
    file_for_results_->flush();
}


particles_energy::particles_energy(
    std::string result_directory, std::string sort_name,
    const Particles& particles)
    : energy_parameters_saver(result_directory),
      particles_(particles) {
  file_for_results_ = std::make_unique<BIN_File>(
    result_directory_, sort_name + "_energy");
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

    W += sqrt(m * m + p.square()) * dx * dy * n / Np;
  }

  file_for_results_->write(W);

  if (t % diagnose_time_step == 0)
    file_for_results_->flush();
}