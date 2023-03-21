#include "simulation_backup.hpp"

#include "src/utils/configuration_backup.hpp"
#include "src/file_writers/bin_file.hpp"

#include "src/particles/particle/point.hpp"

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

  fs::remove_all(result_directory_ + "/timestep_" + std::to_string(t - backup_timestep_));
}

void Simulation_backup::save_particles(int t) const {
  for (const auto& [name, container] : particles_) {

    BIN_File file(result_directory_ + "/timestep_" + std::to_string(t), name);

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
    BIN_File comp_x(result_directory_ + "/timestep_" + std::to_string(t), name + "x");
    BIN_File comp_y(result_directory_ + "/timestep_" + std::to_string(t), name + "y");
    BIN_File comp_z(result_directory_ + "/timestep_" + std::to_string(t), name + "z");

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


/// @todo ENERGY DIAGNOSTIC IS CORRUPTED
size_t Simulation_backup::load() {
  if (!fs::exists(result_directory_)) {
    throw std::runtime_error("Failed to load simulation backup! "
      "No such directory in " + dir_name);
  }

  std::string timestep = get_last_timestep();
  load_particles(timestep);
  load_fields(timestep);

  int from = timestep.find_last_of('/') + 10;  // "timestep_".size() = 10;
  return std::stoul(timestep.substr(from));
}

std::string Simulation_backup::get_last_timestep() {
  for (const auto& directory : fs::directory_iterator(result_directory_)) {
    auto directory_str = directory.path().string();

    if (directory_str.find("timestep_") != std::string::npos)
      return directory_str;
  }

  throw std::runtime_error("Failed to load simulation backup! "
    "No timestep_%% directory in " + result_directory_);
}

void Simulation_backup::load_particles(const std::string& timestep) {
  for (const auto& [name, container] : particles_) {
    std::string filename = timestep + "/" + name + ".bin";
    std::ifstream input_file(filename, std::ios::in | std::ios::binary);

    if (!input_file) {
      throw std::runtime_error("Cannot open file! " + filename);
    }

    size_t size = get_number_of_particles(filename);
    // container.particles_.reserve(size);

    for (size_t i = 0u; i < size; ++i) {
      Point point;

      input_file.read(reinterpret_cast<char*>(&point.x()), sizeof(double));
      input_file.read(reinterpret_cast<char*>(&point.y()), sizeof(double));
      input_file.read(reinterpret_cast<char*>(&point.px()), sizeof(double));
      input_file.read(reinterpret_cast<char*>(&point.py()), sizeof(double));
      input_file.read(reinterpret_cast<char*>(&point.pz()), sizeof(double));

#if GLOBAL_DENSITY
      container.particles_.emplace_back(point, container.parameters_);
#else
      Particle& particle = container.particles_.emplace_back(point, container.parameters_);
      input_file.read(reinterpret_cast<char*>(&particle.n_), sizeof(double));
#endif
    }
  }
}

size_t Simulation_backup::get_number_of_particles(const std::string& filename) {
#if GLOBAL_DENSITY
  size_t number_of_particle_components = 5;
#else
  size_t number_of_particle_components = 6;
#endif

  return fs::file_size(filename) /
    (number_of_particle_components * sizeof(double));
}

void Simulation_backup::load_fields(const std::string& timestep) {
  for (const auto& [name, field] : fields_) {
    std::string filename = timestep + "/" + name;
    std::ifstream comp_x(filename + "x.bin", std::ios::in | std::ios::binary);
    std::ifstream comp_y(filename + "y.bin", std::ios::in | std::ios::binary);
    std::ifstream comp_z(filename + "z.bin", std::ios::in | std::ios::binary);

    if (!comp_x || !comp_y || !comp_z) {
      throw std::runtime_error("Cannot open file! " + filename + "%%.bin");
    }

    for (int ny = 0; ny < SIZE_Y; ++ny) {
    for (int nx = 0; nx < SIZE_X; ++nx) {
      comp_x.read(reinterpret_cast<char*>(&field.x(ny, nx)), sizeof(double));
      comp_y.read(reinterpret_cast<char*>(&field.y(ny, nx)), sizeof(double));
      comp_z.read(reinterpret_cast<char*>(&field.z(ny, nx)), sizeof(double));
    }}
  }
}
