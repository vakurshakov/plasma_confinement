#include "x0_distribution_function.hpp"
#include "src/file_writers/bin_file.hpp"

namespace fs = std::filesystem;

x0_distribution_function::x0_distribution_function(
  std::string result_directory,
  const Particles& particles,
  int x0, diag_area area)
  : distribution_moment(
      result_directory, particles, x0,
      std::make_unique<Projector2D>("VxVy", area)),
    x0_(x0) {}

void x0_distribution_function::save_parameters() const {
  fs::create_directories(fs::path(result_directory_));

  std::ofstream diagnostic_parameters_((result_directory_ +
    "/parameters.txt").c_str(), std::ios::out);

  diagnostic_parameters_ << "#TIME, dt, DTS\n" <<
    TIME << " " << dt << " " << diagnose_time_step << " \n";

  diagnostic_parameters_ << "#x0\n" << x0_ << " \n";

  diagnostic_parameters_ << "#npx_min, npx_max, dpx\n" <<
    min_[X] << " " << max_[X] << " " << projector_->area.dp[X] << " \n";

  diagnostic_parameters_ << "#npy_min, npy_max, dpy\n" <<
    min_[Y] << " " << max_[Y] << " " << projector_->area.dp[Y] << " \n";

  diagnostic_parameters_ << "#sizeof(float)\n" <<
    sizeof(float) << " " << std::endl;
}

void x0_distribution_function::collect() {
  int Np = particles_.get_parameters().Np();

  #pragma omp parallel for
  for (const auto& particle : particles_.get_particles()) {
    if (fabs(particle.point.x() / dx - x0_) > 0.25)
      continue;

    double pr_x = projector_->get_x(particle);
    double pr_y = projector_->get_y(particle);

    int npx = int(round(pr_x / projector_->area.dp[X]));
    int npy = int(round(pr_y / projector_->area.dp[Y]));

    if ((min_[X] < npx && npx < max_[X]) &&
        (min_[Y] < npy && npy < max_[Y])) {
      #pragma omp atomic
      data_[(npy - min_[Y]) * (max_[X] - min_[X]) +
        (npx - min_[X])] += particle.n() / Np;
    }
    else continue;
  }
}
