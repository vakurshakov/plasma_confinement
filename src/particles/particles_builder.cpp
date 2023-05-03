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

  if (!config.contains("Particles")) {
    LOG_WARN("Particles section not found in config file, continuing without them");
    return particles_species;
  }

  config.for_each("Particles", [&](const Configuration_item& description) {
    if (named_species_.contains(description.get("Name"))) {
      throw std::runtime_error("Initialization error: Two particle species share "
        "the same name " + description.get("Name"));
    }

    Particles& particles = particles_species.emplace_back();
    particles.sort_name_ = description.get("Name");

    named_species_.emplace(particles.sort_name_, particles);

    particles.parameters_ = build_parameters(description);

    // Default version of integration steps
    particles.push_ = make_unique<Boris_pusher>();

    particles.interpolation_ = make_unique<Simple_interpolation>(
      particles.parameters_, fields_.E(), fields_.B());

    particles.decomposition_ = make_unique<Esirkepov_density_decomposition>(
      particles.parameters_, fields_.J());

    particles.boundaries_processor_ = make_unique<Particles_boundary>(
      particles.particles_, particles.parameters_, Domain_geometry{});

    if (!description.contains("Integration_steps"))
      LOG_WARN("Integration_steps setting not found for {}, using default ones: "
        "Boris pusher, simple interpolation, Esirkepov density decomposition "
        "and no boundary processor for particles", particles.sort_name_);

    description.for_each("Integration_steps", [&](const Configuration_item& step_description) {
      if (step_description.contains("Pusher")) {
        particles.push_ = build_pusher(step_description);
      }
      else if (step_description.contains("Interpolation")) {
        particles.interpolation_ = build_interpolation(
          step_description, particles.sort_name_);
      }
      else if (step_description.contains("Decomposition")) {
        particles.decomposition_ = build_decomposition(
          step_description, particles.sort_name_);
      }
      else if (step_description.contains("Boundaries_processor")) {
        particles.boundaries_processor_ = build_particles_boundary(
          step_description, particles.sort_name_);
      }
      else {
        throw std::runtime_error("Initialization error: No corresponding integration "
          "step for " + description.get("Name"));
      }
    });
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
Particles_builder::build_interpolation(const Configuration_item& description, const std::string& sort_name) {
#if THERE_ARE_FIELDS
  const Parameters& parameters = named_species_.at(sort_name).get_parameters();

  string setting = description.get("Interpolation");
  std::unique_ptr<Interpolation> interpolation_up;

  if (setting == "Simple_interpolation") {
    interpolation_up = make_unique<Simple_interpolation>(parameters, fields_.E(), fields_.B());
  }
  else if (setting == "Point_interpolation") {
    if (!description.contains("Const_field"))
      throw std::runtime_error("Initialization error: No adder for " + setting);

    setting = "Const_field";
    if (!description.contains(setting + ".E0") &&
        !description.contains(setting + ".B0"))
      throw std::runtime_error("Initialization error: Neither E0 nor B0 is described to add in " + setting);

    vector3 E0, B0;
    if (description.contains(setting + ".E0")) {
      E0.x() = description.get(setting + ".E0.x", 0.0);
      E0.y() = description.get(setting + ".E0.y", 0.0);
      E0.z() = description.get(setting + ".E0.z", 0.0);
    }

    if (description.contains(setting + ".B0")) {
      B0.x() = description.get(setting + ".B0.x", 0.0);
      B0.y() = description.get(setting + ".B0.y", 0.0);
      B0.z() = description.get(setting + ".B0.z", 0.0);
    }

    if (E0 == vector3::zero && B0 == vector3::zero)
      throw std::runtime_error("Initialization error: Both E0 and B0 are zero for " + setting);

    auto adder = make_unique<Const_field_adder>(E0, B0);
    interpolation_up = make_unique<Point_interpolation>(std::move(adder));
  }
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
Particles_builder::build_decomposition(const Configuration_item& description, const std::string& sort_name) {
#if THERE_ARE_FIELDS
  const Parameters& parameters = named_species_.at(sort_name).get_parameters();

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

std::unique_ptr<Particles_boundary>
Particles_builder::build_particles_boundary(const Configuration_item& description, const std::string& sort_name) {
  auto& particles_vec = named_species_.at(sort_name).particles_;
  auto& parameters = named_species_.at(sort_name).parameters_;

  auto get_domain_geometry = [&]() {
    return Domain_geometry{
      description.get<double>("Domain.x_min"),
      description.get<double>("Domain.x_max"),
      description.get<double>("Domain.y_min"),
      description.get<double>("Domain.y_max")
    };
  };

  string setting = description.get("Boundaries_processor");
  std::unique_ptr<Particles_boundary> boundary_processor_up;

  /// @todo rethink after strategy pattern for Particles_boundary
  if (setting == "Particles_boundary") {
    boundary_processor_up = make_unique<Particles_boundary>(
      particles_vec, parameters, Domain_geometry{});
  }
  else if (setting == "Plasma_boundary") {
    boundary_processor_up = make_unique<Plasma_boundary>(
      particles_vec, parameters, get_domain_geometry());
  }
  else if (setting == "Beam_boundary") {
    if (description.contains("Coordinates") &&
        description.get("Coordinates") == "polar") {
      boundary_processor_up = make_unique<POL_Beam_boundary>(
        particles_vec, parameters, Domain_geometry{
          description.get<double>("Domain.r_min"),
          description.get<double>("Domain.r_max"),
          description.get<double>("Domain.phi_min"),
          description.get<double>("Domain.phi_max")
        });
    }
    else {
      boundary_processor_up = make_unique<Beam_boundary>(
        particles_vec, parameters, get_domain_geometry());
    }
  }
  else if (setting == "Reflective_boundary") {
    boundary_processor_up = make_unique<Reflective_boundary>(
      particles_vec, parameters, get_domain_geometry());
  }
  else if (setting == "Plasma_buffer") {
    boundary_processor_up = make_unique<Plasma_buffer>(
      particles_vec, parameters);
  }
  else if (setting == "Beam_buffer") {
    std::string main_beam_name = description.get("from_buffer_to");
    auto& main_beam_vec = named_species_.at(main_beam_name).particles_;
    auto& main_beam_params = named_species_.at(main_beam_name).parameters_;

    boundary_processor_up = make_unique<Beam_buffer>(
      particles_vec, main_beam_vec, main_beam_params, get_domain_geometry());
  }
  else
    throw std::runtime_error("Initialization error: No matching boundary processor");

  return boundary_processor_up;
}
