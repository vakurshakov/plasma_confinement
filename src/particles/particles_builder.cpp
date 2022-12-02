#include "particles_builder.hpp"

#include "src/particles/particles.hpp"
#include "src/particles/particles_load.hpp"
#include "src/particles/particle/particle.hpp"

#include "src/command/set_particles.hpp"
#include "src/command/copy_coordinates.hpp"

#include "src/solvers/Boris_pusher.hpp"
#include "src/solvers/Esirkepov_density_decomposition.hpp"
#include "src/solvers/concrete_point_interpolation.hpp"

using std::vector, std::string, std::function, std::make_unique, std::stod, std::stoi;

Particles_builder::Particles_builder(Fields& fields)
    : fields_(fields) {}

void Particles_builder::set_sort(const string& sort_name) {
  auto it = config::species_description.find(sort_name);
  if (it == config::species_description.end()) {
    throw std::runtime_error("Initialization error: "
      + sort_name + " not found in constants.h");
  }

  sort_name_ = sort_name;
  sort_description_ = it->second;
  sort_parameters_ = get_description("parameters");
  sort_integration_steps_ = get_description("integration_steps");
}

std::vector<std::string>
Particles_builder::get_description(const string& parameter) {
  auto description_it = sort_description_.find(parameter);

  if (description_it == sort_description_.end())
    throw std::runtime_error("Initialization error: Parameter " + parameter + "isn't defined on sort " + sort_name_ + " in constants.h");

  return description_it->second;
}

Parameters Particles_builder::build_parameters() {
  Parameters parameters;
  parameters.n_  = stod(sort_parameters_[0]);
  parameters.q_  = stod(sort_parameters_[1]);
  parameters.m_  = stod(sort_parameters_[2]);
  parameters.Np_ = stoi(sort_parameters_[3]);
  parameters.Tx_ = stod(sort_parameters_[4]);
  parameters.Ty_ = stod(sort_parameters_[5]);
  parameters.Tz_ = stod(sort_parameters_[6]);
  parameters.p0_ = stod(sort_parameters_[7]);

  parameters.sort_name_ = get_sort_name();
  return parameters;
}

std::unique_ptr<Pusher>
Particles_builder::build_pusher() {
  const auto& setting = sort_integration_steps_[0];

  if (setting.find("Boris_pusher:") == string::npos)
    throw std::runtime_error("Initialization error: No matching Particle pusher");

  if ( setting.find("+Push_particle") == string::npos )
    throw std::runtime_error("Initialization error: Known pusher not command");

  return make_unique<Boris_pusher>();
}

std::unique_ptr<Interpolation>
Particles_builder::build_interpolation(const Parameters& parameters) {
  // может возникнуть сложность с одновременным использованием
  // двух интерполяций, нужно будет это поправить.

  size_t pos = string::npos;
  const string& setting = sort_integration_steps_[1];

  std::unique_ptr<Interpolation> interpolation_up;
  if ((pos = setting.find("Boris_pusher:")) != string::npos) {
    if ( setting.find("+Interpolation", pos) == string::npos)
      throw std::runtime_error("Initialization error: Known pusher not command");

    interpolation_up = make_unique<Boris_interpolation>(parameters, fields_.E(), fields_.B());
  }
  else if ((pos = setting.find("Concrete_point_interpolation")) != string::npos ) {
    if ((pos = setting.find("Homogenius_field", pos)) == string::npos)
      throw std::runtime_error("\n\t\t\t\twhat():  Known interpolation not adder");

    pos = setting.find("E0=", pos);
    pos += 3;

    int divider_1 = setting.find(',', pos);
    int divider_2 = setting.find(',', divider_1+1);
    int end     = setting.find(')', divider_2+1);

    double E0x = stod(setting.substr(pos+1, divider_1-(pos+1)));
    double E0y = stod(setting.substr(divider_1+1, divider_2-(divider_1+1)));
    double E0z = stod(setting.substr(divider_2+1, end-(divider_2+1)));

    vector3 E0 = {E0x, E0y, E0z};

    pos = setting.find("B0=", pos);
    pos += 3;

    divider_1 = setting.find(',', pos);
    divider_2 = setting.find(',', divider_1+1);
    end     = setting.find(')', divider_2+1);

    double B0x = stod(setting.substr(pos+1, divider_1-(pos+1)));
    double B0y = stod(setting.substr(divider_1+1, divider_2-(divider_1+1)));
    double B0z = stod(setting.substr(divider_2+1, end-(divider_2+1)));

    vector3 B0 = {B0x, B0y, B0z};

    std::unique_ptr<Local_field_adder> adder = make_unique<Homogenius_field_adder>(E0, B0);
    interpolation_up = make_unique<Concrete_point_interpolation>(std::move(adder));
  }
  else
    throw std::runtime_error("Initialization error: No matching interpolation");

  return interpolation_up;
}

std::unique_ptr<Decomposition>
Particles_builder::build_decomposition(const Parameters& parameters) {
  const std::string& setting = sort_integration_steps_[2];

  if ( setting.find("Esirkepov_density_decomposition") == string::npos )
    throw std::runtime_error("Initialization error: No matching density decomposition");

  return make_unique<Esirkepov_density_decomposition>(parameters, fields_.J());
}
