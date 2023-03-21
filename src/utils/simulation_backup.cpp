#include "simulation_backup.hpp"

#include "src/utils/configuration_backup.hpp"
#include "src/file_writers/bin_file.hpp"

namespace fs = std::filesystem;

Simulation_backup::Simulation_backup(
    int backup_timestep,
    std::unordered_map<std::string, Particles&> each_particles_sort,
    std::unordered_map<std::string, vector3_field&> each_vector_field)
    : Diagnostic(dir_name + "/simulation_backup"),
      backup_timestep_(backup_timestep),
      particles_(each_particles_sort),
      fields_(each_vector_field) {
  save_parameters();
}


void Simulation_backup::save_parameters() const {
  fs::create_directories(result_directory_);

  Configuration_backup config(result_directory_ + "/src/");
  config.save_sources();

  std::ofstream param_file((result_directory_ +
    "/parameters.txt").c_str(), std::ios::out);

  param_file << "# TIME, dt\n";
  param_file << TIME << " " << dt << " \n\n";
  param_file << "# field parameters: SIZE_X, dx, SIZE_Y, dy\n";
  param_file << SIZE_X << " " << dx << " " << SIZE_Y << " " << dy << " \n\n";
  param_file << "# particles global parameters: name, n, q, m, Np, p0, Tx, Ty, Tz\n";

  for (const auto& [name, container] : particles_) {
    const Parameters& params = container.get_parameters();

    param_file
      << name << " "
      << params.n() << " "
      << params.q() << " "
      << params.m() << " "
      << params.Np() << " "
      << params.p0() << " "
      << params.Tx() << " "
      << params.Ty() << " "
      << params.Tz() << " \n";
  }

  param_file << "\n";
  param_file << "# is density global:\n";
  param_file << std::boolalpha << GLOBAL_DENSITY << "\n\n";
  param_file << "# sizeof(double)\n";
  param_file << sizeof(double)<< std::endl;
}

void Simulation_backup::save(int t) {
  diagnose(t);
}

void Simulation_backup::diagnose(int t) {
  PROFILE_FUNCTION();

  if (t % backup_timestep_ != 0) return;

  save_particles(t);
  save_fields(t);

#if LOGGING
  LOG_FLUSH();

  fs::copy(dir_name + "/simulation.log", result_directory_,
    fs::copy_options::overwrite_existing);
#endif

  fs::remove_all(result_directory_ + "/" + std::to_string(t - backup_timestep_));
}

void Simulation_backup::save_particles(int t) const {
  for (const auto& [name, container] : particles_) {

    BIN_File file(result_directory_ + "/" + std::to_string(t), name);

    for (const Particle& particle : container.get_particles()) {
      const Point& point = particle.point;

      file.write_double(point.x());
      file.write_double(point.y());
      file.write_double(point.px());
      file.write_double(point.py());
      file.write_double(point.pz());

#if !GLOBAL_DENSITY
      file.write_double(particle.n_);
#endif
    }

    file.flush();
  }
}

void Simulation_backup::save_fields(int t) const {
  for (const auto& [name, field] : fields_) {
    BIN_File comp_x(result_directory_ + "/" + std::to_string(t), name + "x");
    BIN_File comp_y(result_directory_ + "/" + std::to_string(t), name + "y");
    BIN_File comp_z(result_directory_ + "/" + std::to_string(t), name + "z");

    for (int ny = 0; ny < SIZE_Y; ++ny) {
    for (int nx = 0; nx < SIZE_X; ++nx) {
      comp_x.write_double(field.x(ny, nx));
      comp_y.write_double(field.y(ny, nx));
      comp_z.write_double(field.z(ny, nx));
    }}

    comp_x.flush();
    comp_y.flush();
    comp_z.flush();
  }
}


void Simulation_backup::load() {
  load_particles();
  load_fields();
}

void Simulation_backup::load_particles() {

}

void Simulation_backup::load_fields() {

}


