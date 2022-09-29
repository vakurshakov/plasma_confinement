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

  diagnostic_parameters_ << "#sizeof(float), CHAR_BIT\n" <<
    sizeof(float) << " " << CHAR_BIT << " " << std::endl;
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
}

void distribution_moment::collect() {
  int Np = particles_.get_parameters().Np();

  #pragma omp parallel for
  for (const auto& particle : particles_.get_particles()) {
    double pr_x = projector_->get_x(particle);
    double pr_y = projector_->get_y(particle);

    int npx = int(round(pr_x / projector_->area.dp[X]));
    int npy = int(round(pr_y / projector_->area.dp[Y]));

    if ((min_[X] < npx && npx < max_[X]) &&
        (min_[Y] < npy && npy < max_[Y])) {
      #pragma omp atomic
      data_[(npy - min_[Y]) * (max_[X] - min_[X]) +
        (npx - min_[X])] += moment_->get(particle) / Np;
    }
    else continue;
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
  return particle.n();
}

inline double get_first_Vx_moment(const Particle& particle) {
  return particle.velocity().x();
}

inline double get_first_Vy_moment(const Particle& particle) {
  return particle.velocity().y();
}

inline double get_first_Vr_moment(const Particle& particle) {
  double x = particle.point.x() - 0.5 * SIZE_X * dx;
  double y = particle.point.y() - 0.5 * SIZE_Y * dy;
  double r = sqrt(x * x + y * y);

  // Частицы, близкие к центру не учитываются
  if (!std::isfinite(1. / r)) return 0;

  return particle.velocity().x() * (+x / r) +
    particle.velocity().y() * (y / r);
}

inline double get_first_Vphi_moment(const Particle& particle) {
  double x = particle.point.x() - 0.5 * SIZE_X * dx;
  double y = particle.point.y() - 0.5 * SIZE_Y * dy;
  double r = sqrt(x * x + y * y);

  // Частицы, близкие к центру не учитываются
  if (!std::isfinite(1. / r)) return 0;

  return particle.velocity().x() * (-y / r) +
    particle.velocity().y() * (x / r);
}

Moment::Moment(std::string name) : name(name) {
  if (name == "zeroth_moment") {
    get = get_zeroth_moment;
  }
  else if (name == "first_Vx_moment") {
    get = get_first_Vx_moment;
  }
  else if (name == "first_Vy_moment") {
    get = get_first_Vy_moment;
  }
  else if (name == "first_Vr_moment") {
    get = get_first_Vr_moment;
  }
  else if (name == "first_Vphi_moment") {
    get = get_first_Vphi_moment;
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