#ifndef SRC_DIAGNOSTICS_X0_DISTRIBUTION_FUNCTION_HPP
#define SRC_DIAGNOSTICS_X0_DISTRIBUTION_FUNCTION_HPP

#include "diagnostic.hpp"
#include "src/diagnostics/distribution_moment.hpp"

/// @todo think how to make this diagnostic more general
class x0_distribution_function : public distribution_moment {
 public:
  x0_distribution_function(
    std::string result_directory,
    const Particles& particles,
    int x0, diag_area area);

  void save_parameters() const override;

 private:
  void collect() override;

  int x0_;
};

#endif  // SRC_DIAGNOSTICS_X0_DISTRIBUTION_FUNCTION_HPP
