#ifndef SRC_UTILS_SIMULATION_BACKUP_HPP
#define SRC_UTILS_SIMULATION_BACKUP_HPP

#include "src/pch.h"
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
  size_t get_last_timestep() const;
  static void restore_time_diagnostics();

 private:
  int backup_timestep_;
  std::unordered_map<std::string, Particles&> particles_;
  std::unordered_map<std::string, vector3_field&> fields_;

  void diagnose(int t) override;
  void save_parameters() const override;

  void save_particles(int t) const;
  void save_fields(int t) const;
  void save_time_diagnostics() const;

  std::string get_last_timestep_directory() const;
  void load_particles(const std::string& timestep);
  size_t get_number_of_particles(const std::string& filename);

  void load_fields(const std::string& timestep);
};

#endif  // SRC_UTILS_SIMULATION_BACKUP_HPP
