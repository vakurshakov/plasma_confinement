#include "chosen_particles.hpp"
#include "src/file_writers/bin_file.hpp"

namespace fs = std::filesystem;

// Будем отдельно прицеплять эту диагностику
std::vector<int> choose_indexes(const std::vector<Particle>& particles) {
  std::vector<int> indexes_of_chosen_particles;

  for(size_t i = 0u; i < particles.size(); i += config::Npi) {
    auto& point = particles[i].point;

    double x = round(point.x() / dx);
    double y = round(point.y() / dy);

    if(fabs(x - y) < 1e-6) {
      indexes_of_chosen_particles.emplace_back(i);
    }
  }

  return indexes_of_chosen_particles;
}


chosen_particles::chosen_particles(
    std::string result_directory,
    const Particles& particles,
    std::vector<int> indexes_of_chosen_particles)
    : Diagnostic(result_directory),
      particles_(particles),
      indexes_of_chosen_particles_(std::move(indexes_of_chosen_particles)) {
  save_parameters();

  for(size_t i = 0u; i < indexes_of_chosen_particles_.size(); ++i) {
    files_for_results_.emplace_back(std::make_unique<BIN_File>(
      result_directory_, std::to_string(i) + "_particle"));
  }
}

void chosen_particles::save_parameters() const {
  fs::create_directories(fs::path(result_directory_));

  std::ofstream diagnostic_parameters_((result_directory_ +
    "/parameters.txt").c_str(), std::ios::out);

  diagnostic_parameters_ << "#TIME dt DTS\n";
  diagnostic_parameters_ << TIME << " " << dt << " " << diagnose_time_step << " \n";
  diagnostic_parameters_ << "#there are " << indexes_of_chosen_particles_.size() << " choosen particles\n";
  diagnostic_parameters_ << "#sizeof(float)\n";
  diagnostic_parameters_ << sizeof(float) << std::endl;
}

void chosen_particles::diagnose(int t) {
  PROFILE_FUNCTION();

  int ith_particle = 0;
  for(const auto& i : indexes_of_chosen_particles_) {
    const Point& ith_point = particles_.get_particles()[i].point;

    files_for_results_[ith_particle]->write(ith_point.x());
    files_for_results_[ith_particle]->write(ith_point.y());

    files_for_results_[ith_particle]->write(ith_point.px());
    files_for_results_[ith_particle]->write(ith_point.py());
    files_for_results_[ith_particle]->write(ith_point.pz());

    files_for_results_[ith_particle]->flush();
    ith_particle++;
  }
}
