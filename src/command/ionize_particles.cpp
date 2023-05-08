#include "ionize_particles.hpp"

#include "src/utils/configuration.hpp"
#include "src/utils/random_number_generator.hpp"

Ionize_particles::Ionize_particles(
  Particles* const ionized,
  Particles* const ejected,
  int injection_start,
  int injection_end,
  int per_step_particles_num,
  const Random_coordinate_generator& set_point_of_birth,
  const Density_profile& get_probability,
  const Momentum_generator& load_momentum_i,
  const Momentum_generator& load_momentum_e)
  :	ionized_(ionized),
    ejected_(ejected),
    injection_start_(injection_start),
    injection_end_(injection_end),
    per_step_particles_num_(per_step_particles_num),
    set_point_of_birth_(set_point_of_birth),
    get_probability_(get_probability),
    load_momentum_i_(load_momentum_i),
    load_momentum_e_(load_momentum_e),
#if !START_FROM_BACKUP
    ionized_energy_(BIN_File(Configuration::out_dir(), "ionized_energy")),
    ejected_energy_(BIN_File(Configuration::out_dir(), "ejected_energy"))
#else  // here offset not needed!
    ionized_energy_(BIN_File::from_backup(Configuration::out_dir(), "ionized_energy", 0)),
    ejected_energy_(BIN_File::from_backup(Configuration::out_dir(), "ejected_energy", 0))
#endif
{
  ionized_->particles_.reserve(per_step_particles_num_ * (injection_end_ - injection_start_) + 10'000);
  ejected_->particles_.reserve(per_step_particles_num_ * (injection_end_ - injection_start_) + 10'000);
}

void Ionize_particles::execute(int t) {
  PROFILE_FUNCTION();

  static const int Npi   = ionized_->get_parameters().Np();
  static const double mi = ionized_->get_parameters().m();

  static const int Npe   = ejected_->get_parameters().Np();
  static const double me = ejected_->get_parameters().m();

  if (t < injection_start_)
    return;

  LOG_INFO("Injecting particles, {} particles will be loaded into {} and {}",
    per_step_particles_num_,
    ionized_->get_parameters().get_name(),
    ejected_->get_parameters().get_name());

  /// @todo turn off energy diagnostic via config file
  double loaded_energy_i = 0;
  double loaded_energy_e = 0;

  for (int p = 0; p < per_step_particles_num_; ++p) {
    double x, y;

    #define TO_CELL(c_wp, ds) static_cast<int>(floor(c_wp / ds))
    do {
      set_point_of_birth_(x, y);
    }
    while (random_01() > get_probability_(TO_CELL(x, dx), TO_CELL(y, dy)));
    #undef TO_CELL

    vector2 shared_coordinate{x, y};
    vector3 momentum_i = load_momentum_i_(shared_coordinate);
    vector3 momentum_e = load_momentum_e_(shared_coordinate);

    loaded_energy_i += (sqrt(mi * mi + (momentum_i.square())) - mi) * dx * dy / Npi;
    loaded_energy_e += (sqrt(me * me + (momentum_e.square())) - me) * dx * dy / Npe;

    ionized_->add_particle(Point{shared_coordinate, std::move(momentum_i)});
    ejected_->add_particle(Point{shared_coordinate, std::move(momentum_e)});
  }

  ionized_energy_.write(loaded_energy_i);
  ejected_energy_.write(loaded_energy_e);

  LOG_INFO("Ionized {} energy / step = {}", ionized_->get_parameters().get_name(), loaded_energy_i);
  LOG_INFO("Ejected {} energy / step = {}", ejected_->get_parameters().get_name(), loaded_energy_e);

  if (t % diagnose_time_step == 0) {
    ionized_energy_.flush();
    ejected_energy_.flush();
  }
}
