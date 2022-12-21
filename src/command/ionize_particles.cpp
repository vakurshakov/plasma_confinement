#include "ionize_particles.hpp"

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
    load_impulse(load_impulse) {}

void Ionize_particles::execute(int t) const {
  PROFILE_FUNCTION();
  LOG_INFO("Injecting particles, {} particles will be loaded into {} and {}",
    per_step_particles_num[t-1],  // timestep count starts from 1;
    ionized->get_parameters().get_name(),
    ejected->get_parameters().get_name());

  const double mi =   ionized->get_parameters().m();
  const double Ti_x = ionized->get_parameters().Tx();
  const double Ti_y = ionized->get_parameters().Ty();
  const double Ti_z = ionized->get_parameters().Tz();
  const double pi_0 = ionized->get_parameters().p0();

  const double ml =   ejected->get_parameters().m();
  const double Tl_x = ejected->get_parameters().Tx();
  const double Tl_y = ejected->get_parameters().Ty();
  const double Tl_z = ejected->get_parameters().Tz();
  const double pl_0 = ejected->get_parameters().p0();

  for (size_t i = 0u; i < per_step_particles_num[t-1]; ++i) {
    double x, y;

    do {
      set_point_of_birth(&x, &y);
    }
    while (random_01() > get_probability(x, y));

    double pi_x, pi_y, pi_z;
    double pl_x, pl_y, pl_z;
    do {
      load_impulse(x, y, mi, Ti_x, Ti_y, Ti_z, pi_0, &pi_x, &pi_y, &pi_z);
      load_impulse(x, y, ml, Tl_x, Tl_y, Tl_z, pl_0, &pl_x, &pl_y, &pl_z);
    }
    while (std::isinf(pi_x) || std::isinf(pi_y) || std::isinf(pi_z) ||
          std::isinf(pl_x) || std::isinf(pl_y) || std::isinf(pl_z));

    ionized->add_particle(Point({x, y}, {pi_x, pi_y, pi_z}));
    ejected->add_particle(Point({x, y}, {pl_x, pl_y, pl_z}));
  }
}


double uniform_probability(double /* x_coord */, double /* y_coord */) {
  return 1.;
}

std::vector<size_t> set_time_distribution(size_t t_inj, size_t total_particles_num) {
  std::vector<size_t> array_of_particles_to_load(t_inj);

  for (size_t t = 0u; t < t_inj; ++t) {
    array_of_particles_to_load[t] = 0;
  }

  return array_of_particles_to_load;
}
