#include "particles_builder.hpp"

#include "src/particles/particles.hpp"
#include "src/particles/particles_load.hpp"
#include "src/particles/particle/particle.hpp"

#include "src/solvers/Boris_pusher.hpp"

#include "src/solvers/simple_interpolation.hpp"
#include "src/solvers/point_interpolation.hpp"
#include "src/solvers/null_interpolation.hpp"

#include "src/solvers/Esirkepov_density_decomposition.hpp"
#include "src/solvers/null_decomposition.hpp"

using std::vector, std::string, std::make_unique;

Particles_builder::Particles_builder(Fields& fields)
    : fields_(fields) {}

vector<Particles> Particles_builder::build() {
  PROFILE_FUNCTION();
  LOG_TRACE("Building particles...");

  const Configuration& config = Configuration::instance();

  vector<Particles> particles_species;

  if (!config.contains("Particles"))
    return particles_species;

  config.for_each("Particles", [&](const Configuration_item& description) {
    Particles& particles = particles_species.emplace_back();

    particles.sort_name_ = description.get("Name");
    particles.parameters_ = build_parameters(description);

    particles.push_ = build_pusher(description);
    particles.interpolation_ = build_interpolation(description, particles.parameters_);
    particles.decomposition_ = build_decomposition(description, particles.parameters_);
  });

  return particles_species;
}


Parameters
Particles_builder::build_parameters(const Configuration_item& description) {
  Parameters parameters;
  parameters.Np_ = description.get<int>("Parameters.Np");
  parameters.n_  = description.get<double>("Parameters.density");
  parameters.q_  = description.get<double>("Parameters.charge");
  parameters.m_  = description.get<double>("Parameters.mass");
  parameters.Tx_ = description.get<double>("Parameters.T_x");
  parameters.Ty_ = description.get<double>("Parameters.T_y");
  parameters.p0_ = description.get<double>("Parameters.p0", 0.0);

#if _2D3V
  parameters.Tz_ = description.get<double>("Parameters.T_z");
#endif

  parameters.sort_name_ = description.get("Name");
  return parameters;
}

std::unique_ptr<Pusher>
Particles_builder::build_pusher(const Configuration_item& description) {
  string setting = description.get("Pusher");

  if (setting != "Boris_pusher")
    throw std::runtime_error("Initialization error: No matching Particle pusher");

  return make_unique<Boris_pusher>();
}

std::unique_ptr<Interpolation>
Particles_builder::build_interpolation(
  const Configuration_item& description, const Parameters& parameters) {
#if THERE_ARE_FIELDS
  string setting = description.get("Interpolation");

  std::unique_ptr<Interpolation> interpolation_up;
  if (setting == "Simple_interpolation") {
    interpolation_up = make_unique<Simple_interpolation>(parameters, fields_.E(), fields_.B());
  }
/** @todo fix the problem via config file
*  else if (setting == "Point_interpolation") {
*    if (!description.contains(setting + ".Const_field"))
*      throw std::runtime_error("Initialization error: No adder for " + setting);
*
*    setting += ".Const_field";
*    if (!description.contains(setting + ".E0") &&
*        !description.contains(setting + ".B0"))
*      throw std::runtime_error("Initialization error: Neither E0 nor B0 is described to add in " + setting);
*
*    vector3 E0, B0;
*    if (description.contains(setting + ".E0")) {
*      E0.x() = description.get(setting + ".E0.x", 0.0);
*      E0.y() = description.get(setting + ".E0.y", 0.0);
*      E0.z() = description.get(setting + ".E0.z", 0.0);
*    }
*
*    if (description.contains(setting + ".B0")) {
*      B0.x() = description.get(setting + ".B0.x", 0.0);
*      B0.y() = description.get(setting + ".B0.y", 0.0);
*      B0.z() = description.get(setting + ".B0.z", 0.0);
*    }
*
*    if (E0 == vector3::zero && B0 == vector3::zero)
*      throw std::runtime_error("Initialization error: Both E0 and B0 are zero for " + setting);
*
*    auto adder = make_unique<Const_field_adder>(E0, B0);
*    interpolation_up = make_unique<Point_interpolation>(std::move(adder));
*  }
*/
  else if (setting == "Null_interpolation") {
    interpolation_up = make_unique<Null_interpolation>();
  }
  else
    throw std::runtime_error("Initialization error: No matching interpolation");

  return interpolation_up;
#else
  return make_unique<Null_interpolation>();
#endif
}

std::unique_ptr<Decomposition>
Particles_builder::build_decomposition(
  const Configuration_item& description, const Parameters& parameters) {
#if THERE_ARE_FIELDS
  string setting = description.get("Decomposition");

  std::unique_ptr<Decomposition> decomposition_up;
  if (setting == "Esirkepov_density_decomposition") {
    decomposition_up = make_unique<Esirkepov_density_decomposition>(parameters, fields_.J());
  }
  else if (setting == "Null_decomposition") {
    decomposition_up = make_unique<Null_decomposition>();
  }
  else
    throw std::runtime_error("Initialization error: No matching density decomposition");

  return decomposition_up;
#else
  return make_unique<Null_decomposition>();
#endif
}
