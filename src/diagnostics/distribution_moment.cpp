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

  min_[X] = int(round(projector_->area.min[X] / projector_->area.dp[X]));
  min_[Y] = int(round(projector_->area.min[Y] / projector_->area.dp[Y]));
  max_[X] = int(round(projector_->area.max[X] / projector_->area.dp[X]));
  max_[Y] = int(round(projector_->area.max[Y] / projector_->area.dp[Y]));
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

  min_[X] = int(round(projector_->area.min[X] / projector_->area.dp[X]));
  min_[Y] = int(round(projector_->area.min[Y] / projector_->area.dp[Y]));
  max_[X] = int(round(projector_->area.max[X] / projector_->area.dp[X]));
  max_[Y] = int(round(projector_->area.max[Y] / projector_->area.dp[Y]));
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

  clear();
  collect();

  for (int npy = 0; npy < (max_[Y] - min_[Y]); ++npy) {
  for (int npx = 0; npx < (max_[X] - min_[X]); ++npx) {
    file_for_results_->write(data_[npy * (max_[X] - min_[X]) + npx]);
  }}

  file_for_results_->close();
}

void distribution_moment::collect() {
  const int Np = particles_.get_parameters().Np();

  const double& area_dx = projector_->area.dp[X];
  const double& area_dy = projector_->area.dp[Y];

  #pragma omp parallel for
  for (const auto& particle : particles_.get_particles()) {
    double pr_x = projector_->get_x(particle);
    double pr_y = projector_->get_y(particle);

    int npx = int(round(pr_x / area_dx));
    int npy = int(round(pr_y / area_dy));

    for (int i = npx - shape_radius; i <= npx + shape_radius; ++i) {
    for (int j = npy - shape_radius; j <= npy + shape_radius; ++j) {
      if ((min_[X] <= i && i < max_[X]) && (min_[Y] <= j && j < max_[Y])) {
        #pragma omp atomic
        data_[(j - min_[Y]) * (max_[X] - min_[X]) + (i - min_[X])] +=
          moment_->get(particle) * particle.n() / Np *
          shape_function(pr_x - i * area_dx, area_dx) *
          shape_function(pr_y - j * area_dy, area_dy);
      }
      else continue;
    }}
  }
}

void distribution_moment::clear() {
  #pragma omp parallel for
  for (auto npy = 0; npy < (max_[Y] - min_[Y]); ++npy) {
  for (auto npx = 0; npx < (max_[X] - min_[X]); ++npx) {
    data_[npy * (max_[X] - min_[X]) + npx] = 0;
  }}
}


inline double get_zeroth(const Particle&) {
  return 1.0;
}

inline double get_Vx(const Particle& particle) {
  return particle.velocity().x();
}

inline double get_Vy(const Particle& particle) {
  return particle.velocity().y();
}

inline double get_mVxVx(const Particle& particle) {
  return particle.m() * get_Vx(particle) * get_Vx(particle);
}

inline double get_mVxVy(const Particle& particle) {
  return particle.m() * get_Vx(particle) * get_Vy(particle);
}

inline double get_mVyVy(const Particle& particle) {
  return particle.m() * get_Vy(particle) * get_Vy(particle);
}

inline double get_Vr(const Particle& particle) {
  double x = particle.point.x() - 0.5 * SIZE_X * dx;
  double y = particle.point.y() - 0.5 * SIZE_Y * dy;
  double r = sqrt(x * x + y * y);

  // Particles close to r=0 are not taken into account
  if (std::isinf(1.0 / r))
    return 0.0;

  return (+x * get_Vx(particle) + y * get_Vy(particle)) / r;
}

inline double get_Vphi(const Particle& particle) {
  double x = particle.point.x() - 0.5 * SIZE_X * dx;
  double y = particle.point.y() - 0.5 * SIZE_Y * dy;
  double r = sqrt(x * x + y * y);

  // Particles close to r=0 are not taken into account
  if (std::isinf(1.0 / r))
    return 0.0;

  return (-y * get_Vx(particle) + x * get_Vy(particle)) / r;
}

inline double get_mVrVr(const Particle& particle) {
  return particle.m() * get_Vr(particle) * get_Vr(particle);
}

inline double get_mVrVphi(const Particle& particle) {
  return particle.m() * get_Vr(particle) * get_Vphi(particle);
}

inline double get_mVphiVphi(const Particle& particle) {
  return particle.m() * get_Vphi(particle) * get_Vphi(particle);
}

Moment::Moment(std::string name) : name(name) {
  if (name == "zeroth_moment") {
    get = get_zeroth;
  }
  else if (name == "Vx_moment") {
    get = get_Vx;
  }
  else if (name == "Vy_moment") {
    get = get_Vy;
  }
  else if (name == "mVxVx_moment") {
    get = get_mVxVx;
  }
  else if (name == "mVxVy_moment") {
    get = get_mVxVy;
  }
  else if (name == "mVyVy_moment") {
    get = get_mVyVy;
  }
  else if (name == "Vr_moment") {
    get = get_Vr;
  }
  else if (name == "Vphi_moment") {
    get = get_Vphi;
  }
  else if (name == "mVrVr_moment") {
    get = get_mVrVr;
  }
  else if (name == "mVrVphi_moment") {
    get = get_mVrVphi;
  }
  else if (name == "mVphiVphi_moment") {
    get = get_mVphiVphi;
  }
}


inline double project_to_x(const Particle& particle) {
  return particle.point.x();
}

inline double project_to_y(const Particle& particle) {
  return particle.point.y();
}


Projector2D::Projector2D(std::string axes_names, diag_area area)
    : axes_names(axes_names), area(area) {
  if (axes_names == "XY") {
    get_x = project_to_x;
    get_y = project_to_y;
  }
  else if (axes_names == "VxVy") {
    get_x = get_Vx;
    get_y = get_Vy;
  }
  else if (axes_names == "VrVphi") {
    get_x = get_Vr;
    get_y = get_Vphi;
  }
}
