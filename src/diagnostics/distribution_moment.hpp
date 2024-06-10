#ifndef SRC_DIAGNOSTICS_DISTRIBUTION_MOMENT_HPP
#define SRC_DIAGNOSTICS_DISTRIBUTION_MOMENT_HPP

#include "diagnostic.hpp"
#include "src/particles/particles.hpp"

struct Moment;
struct Projector2D;

class distribution_moment : public Diagnostic {
 public:
  distribution_moment(
    std::string result_directory,
    const Particles& particles,
    std::unique_ptr<Moment>,
    std::unique_ptr<Projector2D>);

  void diagnose(int t) override;

  void save_parameters() const override;

 protected:
  distribution_moment(
    std::string result_directory,
    const Particles& particles,
    int x0, std::unique_ptr<Projector2D>);

  virtual void collect();
  void clear();

  const Particles& particles_;
  std::unique_ptr<Moment> moment_;
  std::unique_ptr<Projector2D> projector_;

  std::vector<double> data_;
  int min_[2];
  int max_[2];
};


/**
 * @brief An utility structure to store the moment name
 * along with the pointer to distribution moment getter.
 *
 * Getter is chosen depending on the name in the constructor.
 */
struct Moment {
  Moment(std::string name);

  std::string name;

  using getter = double (*)(const Particle&);
  getter get = nullptr;
};


struct diag_area {
  double min[2];
  double max[2];
  double dp[2];

  diag_area(
    double min_x, double min_y,
    double max_x, double max_y,
    double dp_x, double dp_y)
    : min{min_x, min_y},
      max{max_x, max_y},
      dp{dp_x, dp_y} {}
};

/**
 * @brief Stores geometry: min and max value of a
 * projection, its step dp and two projectors to
 * axes.
 *
 * Projectors are selected depending on the name in the constructor.
 */
struct Projector2D {
  Projector2D(std::string axes_names, diag_area area);

  std::string axes_names;

  using getter = double (*)(const Particle&);
  getter get_x = nullptr;
  getter get_y = nullptr;

  diag_area area;
};

#endif  // SRC_DIAGNOSTICS_DISTRIBUTION_MOMENT_HPP
