#ifndef SRC_DIAGNOSTICS_CHOSEN_PARTICLES_HPP
#define SRC_DIAGNOSTICS_CHOSEN_PARTICLES_HPP

#include "diagnostic.hpp"
#include "src/particles/particles.hpp"

class chosen_particles : public Diagnostic {
 public:
  chosen_particles(std::string result_directory,
    const Particles& particles,
    std::vector<int> indexes_of_chosen_particles);

  void diagnose(int t) override;

  void save_parameters() const override;

 private:
  const Particles& particles_;
  
  std::vector<int> indexes_of_chosen_particles_;
  std::vector<std::unique_ptr<IFile>> files_for_results_;
};

std::vector<int> choose_indexes(const std::vector<Particle>& particles);

#endif  // SRC_DIAGNOSTICS_CHOSEN_PARTICLES_HPP
