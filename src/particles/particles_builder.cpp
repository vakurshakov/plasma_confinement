#include "particles_builder.hpp"

#include "src/particles/particles.hpp"
#include "src/particles/particles_load.hpp"
#include "src/particles/particle/particle.hpp"

#include "src/command/set_particles.hpp"
#include "src/command/copy_coordinates.hpp"

#include "src/diagnostics/energy.hpp"
#include "src/diagnostics/chosen_particles.hpp"
#include "src/diagnostics/distribution_moment.hpp"

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

  if (description_it == sort_description_.end()) {  
    throw std::runtime_error("Initialization error: Parameter " + parameter
      + "isn't defined on sort " + sort_name_ + " in constants.h");
  }

  return description_it->second;
}

Parameters Particles_builder::build_parameters() {
  size_t pos = string::npos;
  string setting = sort_parameters_[0];

  std::unique_ptr<Variadic_parameter> n;
  if (pos = setting.find("local"); pos != string::npos) {
    n = make_unique<Local_parameter>();
  }
  else if (pos = setting.find("global, "); pos != string::npos) {
    pos += string("global, ").length();
    int end = setting.length();

    double n0 = stod(setting.substr(pos, end - pos));
    n = make_unique<Global_parameter>(n0);
  }

  pos = string::npos;
  setting = sort_parameters_[1];

  std::unique_ptr<Variadic_parameter> q;
  if (pos = setting.find("local"); pos != string::npos) {
    q = make_unique<Local_parameter>();
  }
  else if (pos = setting.find("global, "); pos != string::npos) {
    pos += string("global, ").length();
    int end = setting.length();

    double q0 = stod(setting.substr(pos, end - pos));
    q = make_unique<Global_parameter>(q0);
  }

  return Parameters(
    stoi(sort_parameters_[3]),
    stod(sort_parameters_[2]),
    std::move(n), std::move(q),
    stod(sort_parameters_[7]),
    stod(sort_parameters_[4]),
    stod(sort_parameters_[5]),
    stod(sort_parameters_[6]));
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

std::vector<std::unique_ptr<Particles_diagnostic>>
Particles_builder::build_diagnostics() {
  std::vector<std::unique_ptr<Particles_diagnostic>> particles_diagnostics = {};

#if particles_are_diagnosed
  for (const auto& [diag, description] : sort_description_) {
    if (diag == "density") {
      particles_diagnostics.emplace_back(make_unique<distribution_moment>(
        dir_name + "/" + sort_name_,
        stod(description[0]), stod(description[1]), stod(description[2]),
        stod(description[3]), stod(description[4]), stod(description[5]),
        make_unique<XY_projector>(),
        make_unique<zeroth_moment>()
      ));
    }
    else if (diag == "1_of_VxVy") {
      std::cout << "Warning: predefined boundary values for 1_of_VxVy will be used" << std::endl;
      particles_diagnostics.emplace_back(make_unique<distribution_moment>(
        dir_name + "/" + sort_name_,
        -1.0,  +1.0,  0.01,
        -1.0,  +1.0,  0.01,
        make_unique<VxVy_projector>(),
        make_unique<zeroth_moment>()));
    }
    else if (diag == "first_Vx_moment") {
      particles_diagnostics.emplace_back(make_unique<distribution_moment>(
        dir_name + "/" + sort_name_,
        stod(description[0]), stod(description[1]), stod(description[2]),
        stod(description[3]), stod(description[4]), stod(description[5]),
        make_unique<XY_projector>(),
        make_unique<first_Vx_moment>()));
    }
    else if (diag == "first_Vy_moment") {
      particles_diagnostics.emplace_back(make_unique<distribution_moment>(
        dir_name + "/" + sort_name_,
        stod(description[0]), stod(description[1]), stod(description[2]),
        stod(description[3]), stod(description[4]), stod(description[5]),
        make_unique<XY_projector>(),
        make_unique<first_Vy_moment>()));
    }
    else if (diag == "first_Vr_moment") {
      particles_diagnostics.emplace_back(make_unique<distribution_moment>(
        dir_name + "/" + sort_name_,
        stod(description[0]), stod(description[1]), stod(description[2]),
        stod(description[3]), stod(description[4]), stod(description[5]),
        make_unique<XY_projector>(),
        make_unique<first_Vr_moment>()));
    }
    else if (diag == "first_Vphi_moment") {
      particles_diagnostics.emplace_back(make_unique<distribution_moment>(
        dir_name + "/" + sort_name_,
        stod(description[0]), stod(description[1]), stod(description[2]),
        stod(description[3]), stod(description[4]), stod(description[5]),
        make_unique<XY_projector>(),
        make_unique<first_Vphi_moment>()));
    }
    else if (diag == "energy") {
      particles_diagnostics.emplace_back(make_unique<particles_energy>(
        dir_name + "/" + sort_name_ + "/" + diag));
    }
  }
  particles_diagnostics.shrink_to_fit();
#endif

  return particles_diagnostics;
}
