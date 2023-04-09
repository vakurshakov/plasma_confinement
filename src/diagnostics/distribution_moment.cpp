#include "distribution_moment.hpp"
#include "src/file_writers/bin_file.hpp"

namespace fs = std::filesystem;

distribution_moment::distribution_moment(
    std::string result_directory,
    const Particles& particles,
    std::unique_ptr<Moment> moment,
    std::unique_ptr<Projector2D> projector)
    : Diagnostic(result_directory + "/" +
      moment->name + "_of_" + projector->axes_names),
      particles_(particles) {
  moment_ = std::move(moment);
  projector_ = std::move(projector);

  min_[X] = int(projector_->area.min[X] / projector_->area.dp[X]);
  min_[Y] = int(projector_->area.min[Y] / projector_->area.dp[Y]);
  max_[X] = int(projector_->area.max[X] / projector_->area.dp[X]);
  max_[Y] = int(projector_->area.max[Y] / projector_->area.dp[Y]);
  data_.reserve((max_[X] - min_[X]) * (max_[Y] - min_[Y]));

  save_parameters();
}

distribution_moment::distribution_moment(
  std::string result_directory,
  const Particles& particles,
  int x0, std::unique_ptr<Projector2D> projector)
  : Diagnostic(result_directory + "/x0=" + std::to_string(x0) + "_distribution_function"),
    particles_(particles) {
  moment_ = nullptr;
  projector_ = std::move(projector);

  min_[X] = int(projector_->area.min[X] / projector_->area.dp[X]);
  min_[Y] = int(projector_->area.min[Y] / projector_->area.dp[Y]);
  max_[X] = int(projector_->area.max[X] / projector_->area.dp[X]);
  max_[Y] = int(projector_->area.max[Y] / projector_->area.dp[Y]);
  data_.reserve((max_[X] - min_[X]) * (max_[Y] - min_[Y]));

  save_parameters();
}

void distribution_moment::save_parameters() const {
  fs::create_directories(fs::path(result_directory_));

  std::ofstream diagnostic_parameters_((result_directory_ +
    "/parameters.txt").c_str(), std::ios::out);

  diagnostic_parameters_ << "#TIME, dt, DTS\n" <<
    TIME << " " << dt << " " << diagnose_time_step << " \n";

  diagnostic_parameters_ << "#npx_min, npx_max, dpx\n" <<
    min_[X] << " " << max_[X] << " " << projector_->area.dp[X] << " \n";

  diagnostic_parameters_ << "#npy_min, npy_max, dpy\n" <<
    min_[Y] << " " << max_[Y] << " " << projector_->area.dp[Y] << " \n";

  diagnostic_parameters_ << "#sizeof(float)\n" <<
    sizeof(float) << " " << std::endl;
}

void distribution_moment::diagnose(int t) {
  PROFILE_FUNCTION();

  if (t % diagnose_time_step != 0) return;

  file_for_results_ = std::make_unique<BIN_File>(
    result_directory_, std::to_string(t));

  collect();

  for (int npy = 0; npy < (max_[Y] - min_[Y]); ++npy) {
  for (int npx = 0; npx < (max_[X] - min_[X]); ++npx) {
    file_for_results_->write(data_[npy * (max_[X] - min_[X]) + npx]);
  }}

  reset();

  file_for_results_->close();
}

void distribution_moment::collect() {
  const int Np = particles_.get_parameters().Np();
  const int width = particles_.get_parameters().charge_cloud();
  const auto& shape = particles_.get_parameters().form_factor();

  #pragma omp parallel for
  for (const auto& particle : particles_.get_particles()) {
    double pr_x = projector_->get_x(particle);
    double pr_y = projector_->get_y(particle);

    // floor(x) here to avoid problems on borders with rounding up and down
    int npx = int(floor(pr_x / projector_->area.dp[X]));
    int npy = int(floor(pr_y / projector_->area.dp[Y]));

    for (int i = npx - width; i <= npx + width; ++i) {
    for (int j = npy - width; j <= npy + width; ++j) {
      if ((min_[X] <= i && i < max_[X]) &&
          (min_[Y] <= j && j < max_[Y])) {
        #pragma omp atomic
        data_[(j - min_[Y]) * (max_[X] - min_[X]) + (i - min_[X])] +=
          moment_->get(particle) * particle.n() / Np *
          shape(pr_x - i * dx, dx) *
          shape(pr_y - j * dy, dy);
      }
      else continue;
    }}
  }
}

void distribution_moment::reset() {
  #pragma omp parallel for
  for (auto npy = 0; npy < (max_[Y] - min_[Y]); ++npy) {
  for (auto npx = 0; npx < (max_[X] - min_[X]); ++npx) {
    data_[npy * (max_[X] - min_[X]) + npx] = 0;
  }}
}


inline double get_zeroth_moment(const Particle& particle) {
  return 1.0;
}

inline double get_Vx_moment(const Particle& particle) {
  return particle.velocity().x();
}

inline double get_Vy_moment(const Particle& particle) {
  return particle.velocity().y();
}

inline double get_mVxVx_moment(const Particle& particle) {
  return particle.m() * get_Vx_moment(particle) * get_Vx_moment(particle);
}

inline double get_mVxVy_moment(const Particle& particle) {
  return particle.m() * get_Vx_moment(particle) * get_Vy_moment(particle);
}

inline double get_mVyVy_moment(const Particle& particle) {
  return particle.m() * get_Vy_moment(particle) * get_Vy_moment(particle);
}

inline double get_Vr_moment(const Particle& particle) {
  double x = particle.point.x() - 0.5 * SIZE_X * dx;
  double y = particle.point.y() - 0.5 * SIZE_Y * dy;
  double r = sqrt(x * x + y * y);

  // Particles close to r=0 are not taken into account
  if (std::isinf(1.0 / r))
    return 0.0;

  return (+x * particle.velocity().x() + y * particle.velocity().y()) / r;
}

inline double get_Vphi_moment(const Particle& particle) {
  double x = particle.point.x() - 0.5 * SIZE_X * dx;
  double y = particle.point.y() - 0.5 * SIZE_Y * dy;
  double r = sqrt(x * x + y * y);

  // Particles close to r=0 are not taken into account
  if (std::isinf(1.0 / r))
    return 0.0;

  return (-y * particle.velocity().x() + x * particle.velocity().y()) / r;
}

inline double get_mVrVr_moment(const Particle& particle) {
  return particle.m() * get_Vr_moment(particle) * get_Vr_moment(particle);
}

inline double get_mVrVphi_moment(const Particle& particle) {
  return particle.m() * get_Vr_moment(particle) * get_Vphi_moment(particle);
}

inline double get_mVphiVphi_moment(const Particle& particle) {
  return particle.m() * get_Vphi_moment(particle) * get_Vphi_moment(particle);
}

Moment::Moment(std::string name) : name(name) {
  if (name == "zeroth_moment") {
    get = get_zeroth_moment;
  }
  else if (name == "Vx_moment") {
    get = get_Vx_moment;
  }
  else if (name == "Vy_moment") {
    get = get_Vy_moment;
  }
  else if (name == "mVxVx_moment") {
    get = get_mVxVx_moment;
  }
  else if (name == "mVxVy_moment") {
    get = get_mVxVy_moment;
  }
  else if (name == "mVyVy_moment") {
    get = get_mVyVy_moment;
  }
  else if (name == "Vr_moment") {
    get = get_Vr_moment;
  }
  else if (name == "Vphi_moment") {
    get = get_Vphi_moment;
  }
  else if (name == "mVrVr_moment") {
    get = get_mVrVr_moment;
  }
  else if (name == "mVrVphi_moment") {
    get = get_mVrVphi_moment;
  }
  else if (name == "mVphiVphi_moment") {
    get = get_mVphiVphi_moment;
  }
}


inline double project_to_x(const Particle& particle) {
  return particle.point.x();
}

inline double project_to_y(const Particle& particle) {
  return particle.point.y();
}

inline double project_to_vx(const Particle& particle) {
  return particle.velocity().x();
}

inline double project_to_vy(const Particle& particle) {
  return particle.velocity().y();
}

Projector2D::Projector2D(std::string axes_names, diag_area area)
    : axes_names(axes_names), area(area) {
  if (axes_names == "XY") {
    get_x = project_to_x;
    get_y = project_to_y;
  }
  else if (axes_names == "VxVy") {
    get_x = project_to_vx;
    get_y = project_to_vy;
  }
}
