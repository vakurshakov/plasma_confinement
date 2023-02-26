#include "ionize_particles.hpp"

#if BEAM_INJECTION_SETUP && GLOBAL_DENSITY

#include "src/utils/random_number_generator.hpp"

Ionize_particles::Ionize_particles(
  Particles* const ionized, Particles* const ejected,
  const std::vector<size_t>& per_step_particles_num,
  std::function<void(double* x, double* y)>&& set_point_of_birth,
  std::function<double(double x, double y)>&& get_probability,
  const impulse_loader& load_impulse)
  :	ionized(ionized), ejected(ejected),
    per_step_particles_num(per_step_particles_num),
    set_point_of_birth(std::move(set_point_of_birth)),
    get_probability(std::move(get_probability)),
    load_impulse(load_impulse),
    ionized_energy(BIN_File(dir_name, "ionized_energy")),
    ejected_energy(BIN_File(dir_name, "ejected_energy")) {}

void Ionize_particles::execute(int t) const {
  PROFILE_FUNCTION();

  if (t < config::INJECTION_START)
    return;

  LOG_INFO("Injecting particles, {} particles will be loaded into {} and {}",
    per_step_particles_num[t-1],  // timestep count starts from 1;
    ionized->get_parameters().get_name(),
    ejected->get_parameters().get_name());

  const double mi =   ionized->get_parameters().m();
  const double Ti_x = ionized->get_parameters().Tx();
  const double Ti_y = ionized->get_parameters().Ty();
  const double Ti_z = ionized->get_parameters().Tz();
  const double pi_0 = ionized->get_parameters().p0();

  const double me =   ejected->get_parameters().m();
  const double Te_x = ejected->get_parameters().Tx();
  const double Te_y = ejected->get_parameters().Ty();
  const double Te_z = ejected->get_parameters().Tz();
  const double pe_0 = ejected->get_parameters().p0();

  double Wi = 0;
  double We = 0;

  for (size_t i = 0u; i < per_step_particles_num[t-1]; ++i) {
    double x, y;

    do {
      set_point_of_birth(&x, &y);
    }
    while (random_01() > get_probability(x, y));

    double pi_x, pi_y, pi_z;
    double pe_x, pe_y, pe_z;
    do {
      load_impulse(x, y, mi, Ti_x, Ti_y, Ti_z, pi_0, &pi_x, &pi_y, &pi_z);
      load_impulse(x, y, me, Te_x, Te_y, Te_z, pe_0, &pe_x, &pe_y, &pe_z);
    }
    while (std::isinf(pi_x) || std::isinf(pi_y) || std::isinf(pi_z) ||
           std::isinf(pe_x) || std::isinf(pe_y) || std::isinf(pe_z));

    Wi += (sqrt(mi * mi + (pi_x * pi_x + pi_y * pi_y + pi_z * pi_z)) - mi) * dx * dy / config::Npi;
    We += (sqrt(me * me + (pe_x * pe_x + pe_y * pe_y + pe_z * pe_z)) - me) * dx * dy / config::Npi;

    ionized->add_particle(Point({x, y}, {pi_x, pi_y, pi_z}));
    ejected->add_particle(Point({x, y}, {pe_x, pe_y, pe_z}));
  }

  ionized_energy.write(Wi);
  ejected_energy.write(We);

  LOG_INFO("Ionized {} energy / step = {}", ionized->get_parameters().get_name(), Wi);
  LOG_INFO("Ejected {} energy / step = {}", ejected->get_parameters().get_name(), We);

  if (t % diagnose_time_step == 0) {
    ionized_energy.flush();
    ejected_energy.flush();
  }
}


double uniform_probability(double /* x_coord */, double /* y_coord */) {
  return 1.;
}

std::vector<size_t> set_time_distribution(size_t t_inj, size_t total_particles_num) {
  std::vector<size_t> array_of_particles_to_load(t_inj);

  for (size_t t = 0u; t < t_inj; ++t) {
    array_of_particles_to_load[t] = config::PER_STEP_PARTICLES;
  }

  return array_of_particles_to_load;
}

#endif
