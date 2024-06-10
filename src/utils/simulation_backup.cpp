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
    const Parameters& params = container.parameters_;

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
  param_file << sizeof(double) << std::endl;
}

void Simulation_backup::save(int t) {
  diagnose(t);
}

void Simulation_backup::diagnose(int t) {
  PROFILE_FUNCTION();

  if (t % backup_timestep_ != 0) return;

  save_particles(t);
  save_fields(t);
  save_time_diagnostics();

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

void Simulation_backup::save_time_diagnostics() const {
  for (const auto& entry : fs::recursive_directory_iterator(dir_name)) {
    auto entry_str = entry.path().string();

    if (entry_str.find(result_directory_) != std::string::npos ||
        entry_str.find(".log") != std::string::npos ||
        entry_str.find(".png") != std::string::npos ||
        entry_str.find(".pdf") != std::string::npos ||
        entry_str.find(".py") != std::string::npos ||
        entry_str.find(".sh") != std::string::npos) {
      continue;
    }

    if (entry_str.find("energy") != std::string::npos ||
        entry_str.find("point_") != std::string::npos) {
      auto begin = dir_name.size() + 1;
      auto end = entry_str.find_last_of("/") + 1;

      auto path = entry_str.substr(begin, (end - begin));
      auto name = entry_str.substr(end);

      const auto diagnostics = result_directory_ + "/time_diagnostics/";
      fs::create_directories(diagnostics + path);

      fs::copy(entry_str, diagnostics + path + "/" + name,
        fs::copy_options::overwrite_existing);
    }
  }
}


void Simulation_backup::load() {
  if (!fs::exists(result_directory_)) {
    throw std::runtime_error("Failed to load simulation backup! "
      "No such directory in " + dir_name);
  }

  std::string timestep = get_last_timestep_directory();
  load_particles(timestep);
  load_fields(timestep);

  /// @todo check current solution, many cells not satisfy the equation div E = q
  /// check_initial_condition();
}

size_t Simulation_backup::get_last_timestep() const {
  std::string timestep = get_last_timestep_directory();
  int from = timestep.find_last_of("/") + 10;  // "timestep_".size() = 10;
  return std::stoul(timestep.substr(from));
}

std::string Simulation_backup::get_last_timestep_directory() const {
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

/* static */ void Simulation_backup::restore_time_diagnostics() {
  bool should_try = false;

  for (const auto& [_1, sort_description] : config::species_description) {
  for (const auto& [diag, _2] : sort_description) {
    if (diag == "energy")
      should_try = true;
  }}

  for (const auto& [name, _] : config::fields_diagnostics) {
    if (name == "energy" || name == "field_at_point")
      should_try = true;
  }

  if (!should_try)
    return;

  const std::string diagnostics = dir_name + "/simulation_backup/time_diagnostics/";

  for (const auto& entry : fs::recursive_directory_iterator(diagnostics)) {
    if (fs::is_directory(entry))
      continue;

    auto entry_str = entry.path().string();
    auto begin = diagnostics.size();
    auto path = entry_str.substr(begin);

    if (path.find("/") != std::string::npos) {
      fs::create_directories(dir_name + "/" + path.substr(0, path.find_last_of("/")));
    }
    fs::copy(entry, dir_name + "/" + path, fs::copy_options::overwrite_existing);
  }
}

constexpr int index(int ny, int nx) {
  return ny * SIZE_X + nx;
}

void Simulation_backup::check_initial_condition() {
  scalar_field div_E(SIZE_X * SIZE_Y, 0.0);
  scalar_field div_B(SIZE_X * SIZE_Y, 0.0);
  scalar_field charge_density(SIZE_X * SIZE_Y, 0.0);

  collect_charge_density(charge_density);
  collect_divergence(div_E, div_B);

  for (int ny = 0; ny < SIZE_Y; ++ny) {
  for (int nx = 0; nx < SIZE_X; ++nx) {
    if (fabs(div_E[index(ny, nx)] - charge_density[index(ny, nx)]) > cell_epsilon) {
      throw std::runtime_error("Initial condition: div E(x, y) = q(x, y) not"
        "satisfied for cell (" + std::to_string(nx) + ", " + std::to_string(ny) + ")!");
    }

    if (fabs(div_B[index(ny, nx)]) > cell_epsilon) {
      throw std::runtime_error("Initial condition: div B(x, y) = 0 not satisfied "
        "for cell (" + std::to_string(nx) + ", " + std::to_string(ny) + ")!");
    }
  }}
}

void Simulation_backup::collect_charge_density(scalar_field& charge_density) {
  for (const auto& [_, container] : particles_) {
    const int Np = container.parameters_.Np();

    for (const auto& particle : container.get_particles()) {
      double x = particle.point.x();
      double y = particle.point.y();

      int nx = int(round(x / dx));
      int ny = int(round(y / dy));

      for (int i = nx - shape_radius; i <= nx + shape_radius; ++i) {
      for (int j = ny - shape_radius; j <= ny + shape_radius; ++j) {
        if ((0 <= i && i < SIZE_X) && (0 <= j && j < SIZE_Y)) {
          charge_density[index(ny, nx)] +=
            particle.n() * particle.q() / Np *
            shape_function(x - i * dx, dx) *
            shape_function(y - j * dy, dy);
        }
        else continue;
      }}
    }
  }
}

void Simulation_backup::collect_divergence(scalar_field& div_E, scalar_field& div_B) {
  vector3_field& E = fields_.at("E");
  vector3_field& B = fields_.at("B");

  for (int ny = 0; ny < SIZE_Y; ++ny) {
  for (int nx = 0; nx < SIZE_X; ++nx) {
    div_E[index(ny, nx)] =
      (E.x(ny, nx) - E.x(ny, nx-1)) / dx +
      (E.y(ny, nx) - E.y(ny-1, nx)) / dy;

    div_B[index(ny, nx)] =
      (B.x(ny, nx) - B.x(ny, nx-1)) / dx +
      (B.y(ny, nx) - B.y(ny-1, nx)) / dy;
  }}
}
