#ifndef SRC_UTILS_SIMULATION_BACKUP_HPP
#define SRC_UTILS_SIMULATION_BACKUP_HPP

#include "src/diagnostics/diagnostic.hpp"
#include "src/particles/particles.hpp"
#include "src/vectors/vector3_field.hpp"

class Simulation_backup : public Diagnostic {
 public:
  Simulation_backup(
    int backup_timestep,
    std::unordered_map<std::string, Particles&> each_particles_sort,
    std::unordered_map<std::string, vector3_field&> each_vector_field);

  void save(int t);
  void load();

 private:
  int backup_timestep_;
  std::unordered_map<std::string, Particles&> particles_;
  std::unordered_map<std::string, vector3_field&> fields_;

  void diagnose(int t) override;
  void save_parameters() const override;

  void save_particles(int t) const;
  void save_fields(int t) const;

  void load_particles();
  void load_fields();
};

#endif  // SRC_UTILS_SIMULATION_BACKUP_HPP
