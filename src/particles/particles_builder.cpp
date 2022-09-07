#include "particles_builder.hpp"

#include "particles.hpp"
#include "particles_load.hpp"
#include "particle/particle.hpp"

#include "../command/set_particles.hpp"
#include "../command/copy_coordinates.hpp"
#include "../diagnostics/energy.hpp"
#include "../diagnostics/chosen_particles.hpp"
#include "../diagnostics/distribution_moment.hpp"
#include "../fields/fields.hpp"
#include "../solvers/Boris_pusher.hpp"
#include "../solvers/Esirkepov_density_decomposition.hpp"
#include "../solvers/concrete_point_interpolation.hpp"
#include "../constants.h"

using std::vector, std::string, std::function, std::make_unique, std::stod, std::stoi;

enum PCONF { push = 0, interpolation, decomposition,
			sort_density, sort_charge, sort_mass, number_of_particles_in_cell,
			command_type, configuration, cell_filling,
			temperature_x, temperature_y, temperature_z,
			initial_impulse };


auto Particles_builder::config_parameters(const vector<string> parameters)
{
	std::cout << "\t\t\tSetting parameters...";

	size_t pos = string::npos;
	string setting = parameters[PCONF::sort_density];

	std::unique_ptr<Variadic_parameter> n;
	if (pos = setting.find("local"); pos != string::npos)
	{
		n = make_unique<Local_parameter>();
	}	
	else if (pos = setting.find("global, "); pos != string::npos)
	{
		pos += string("global, ").length();
		int end = setting.length();

		double n0 = stod(setting.substr(pos, end-pos));
		n = make_unique<Global_parameter>(n0);
	}

	pos = string::npos;
	setting = parameters[PCONF::sort_charge];

	std::unique_ptr<Variadic_parameter> q;
	if (pos = setting.find("local"); pos != string::npos)
	{
		q = make_unique<Local_parameter>();
	}	
	else if (pos = setting.find("global, "); pos != string::npos)
	{
		pos += string("global, ").length();
		int end = setting.length();

		double q0 = stod(setting.substr(pos, end-pos));
		q = make_unique<Global_parameter>(q0);
	}

	Parameters param = Parameters(
		stoi(parameters[PCONF::number_of_particles_in_cell]),
		stod(parameters[PCONF::sort_mass]),
		std::move(n), std::move(q),
		stod(parameters[PCONF::initial_impulse]),
		stod(parameters[PCONF::temperature_x]), 
		stod(parameters[PCONF::temperature_y]), 
		stod(parameters[PCONF::temperature_z])
	);

	std::cout
	<< "\n\t\t\t\t"
		<< "Np=" << parameters[PCONF::number_of_particles_in_cell] << ", "
		<< "m=" << parameters[PCONF::sort_mass] 	<< ", "
		<< "n=" << parameters[PCONF::sort_density] 	<< ", "
		<< "e=" << parameters[PCONF::sort_charge] 	<< ", "
	<< "\n\t\t\t\t"
		<< "p0=" << parameters[PCONF::initial_impulse] 	<< ", "
		<< "Tx=" << parameters[PCONF::temperature_x] 	<< ", "
		<< "Ty=" << parameters[PCONF::temperature_y] 	<< ", "
		<< "Tz=" << parameters[PCONF::temperature_z] 	<< ";";
	std::cout << "\n\t\t\tdone (" << &param << ")" << std::endl;

	return param;
}


auto get_configuration_functions(string configuration)
{
	std::tuple<
	  function<bool(int cell_number_nx, int cell_number_ny)>,
      function<int(int Np)>,
      function<void(double x, double y,
	  	double mass, double Tx, double Ty, double Tz,
	  	double p0, double* px, double* py, double* pz)>> temp;

	if (configuration == "ring")
	{
		temp = std::make_tuple(
				cell_on_a_ring,
				get_number_of_particles_on_ring,
				load_annular_impulse);
	}
	else if (configuration == "circle")
	{
		temp = std::make_tuple(
				cell_in_a_circle,
				get_number_of_particles_in_circle,
				load_uniform_impulse);
	}
	else if (configuration == "single_particle")
	{
		// Решение для тестов, при этом лучше положить Np = 1.
		temp = std::make_tuple(
			[](int x, int y) -> bool {
				return x == SIZE_X/2 && y == SIZE_Y/2;
			},

			[](int) -> int { return 1; },

			[](double x, double y,
			   double mass, double Tx, double Ty, double Tz,
			   double p0, double* px, double* py, double* pz) -> void
			{
				*px = 0;
				*py = 1e-3;
				*pz = 0;
			});
	}
	else if (configuration == "two_particles")
	{
		// Так можно проверить race-condition декомпозиции. 
		temp = std::make_tuple(
			[](int x, int y) -> bool {
				return 	(x == (SIZE_X/2 - 1) && y == SIZE_Y/2) ||
						(x == (SIZE_X/2 + 1) && y == SIZE_Y/2); 
			},
			
			[](int) -> int { return 2; },

			[](double x, double y,
			   double mass, double Tx, double Ty, double Tz,
			   double p0, double* px, double* py, double* pz)
			{
				*px = 0;
				*py = 1e-1;
				*pz = 0;
			});	
	}
	
	return temp;
}

void Particles_builder::load_particles(
	Particles* const particles,
	const vector<string> distribution,
	const Parameters& parameters,
	std::list<Command_up>& settings_before_main_cycle)
{
	std::cout << "\t\t\tLoading points...";

	string command_type 	= distribution[PCONF::command_type];
	string configuration 	= distribution[PCONF::configuration];
	string cell_filling 	= distribution[PCONF::cell_filling];
		
	std::cout << "\n\t\t\t\t" << command_type
		<< ", configuration: " << configuration
		<< ", way of filling cell: " << cell_filling << ";";

	if (command_type == "set")
	{
		auto [this_is_config_cell, get_number_of_particles, load_impulse]
				= get_configuration_functions(configuration);

		function<void(int sequential_number, int Np,
			int cell_number_nx, int cell_number_ny,
			double* x, double* y)> fill_the_cell = nullptr;
	
		if (cell_filling == "random") {
			fill_the_cell = fill_randomly;
		}
		else if (cell_filling == "periodic") {
			fill_the_cell = fill_periodically;
		}

		const int Np = parameters.Np();
		
		settings_before_main_cycle.push_back(make_unique<Set_particles>(
			particles,
			get_number_of_particles(Np),
			this_is_config_cell,
			load_impulse,
			fill_the_cell));
	}
	else if (command_type.starts_with("copy_coordinates"))
	{
		// 	This particles will wait until the end of
		//	the cycle and copy coordinates %from_... 
		throw string(command_type + "," + configuration);
	}
	else if (command_type == "ionize_particles")
	{
		// 	This particles will wait until the end of
		//	the cycle and make a pair into Ionize_particles command
		throw string(command_type + "," + configuration);
	}
	else{
		std::cout << "There is no know command type." << std::endl;
	}
}


auto Particles_builder::x_boundary()
{	
	function<void(Point&, double)> x_boundary = nullptr;
	
	const size_t right_boundary = 2;
	
	// Возвращает необходимую для частиц границу по икс [ работает с файлом ./constants.h ]
	if ( boundaries.empty() ) {
		std::cout << "x_boundary:  Initialization error: No boundaries in file [./constants.h]" << std::endl;
	}
	else if ( boundaries.substr(0, right_boundary) == "rx" ) {
		x_boundary = reflective_Xboundary;
	}
	else if ( boundaries.substr(0, right_boundary) == "px" ) {
		x_boundary = periodic_Xboundary;
	}
	else {
		std::cout << "x_boundary:  Initialization error: No matching boundaries" << std::endl;
	}
	return x_boundary;
}


auto Particles_builder::y_boundary()
{
	function<void(Point&, double)> y_boundary = nullptr;
	
	const size_t left_boundary = 3;
	
	// Возвращает необходимую для частиц границу по игрек [ работает с файлом ./constants.h ]
	if ( boundaries.empty() ) {
		std::cout << "y_boundary:  Initialization error: No boundaries in file [./constants.h]" << std::endl;
	}
	else if ( boundaries.substr(left_boundary, boundaries.size()) == "ry" ) {
		y_boundary = reflective_Yboundary;
	}
	else if ( boundaries.substr(left_boundary, boundaries.size()) == "py" ) {
		y_boundary = periodic_Yboundary;
	}
	else {
		std::cout << "y_boundary:  Initialization error: No matching boundaries" << std::endl;
	}
	return y_boundary;
}


auto Particles_builder::choose_pusher(const vector<string> description,
	const Parameters& parameters, Fields& fields)
{
	std::cout << "\t\t\tSetting pusher... ";	
	std::unique_ptr<Pusher> pusher_up;

	const auto& setting = description[PCONF::push];		

	if (setting.find("Boris_pusher:") != string::npos)
	{
		if ( setting.find("+Push_particle") != string::npos )
		{
			pusher_up = make_unique<Boris_pusher>();
			std::cout << "done" << std::endl;
		}
		else {
			std::cout << "\n\t\t\t\twhat():  Known pusher not command" << std::endl;	
		}
	}
	else {
		std::cout << "\n\t\t\t\twhat():  Initialization error: No matching Particle pusher" << std::endl;	
	}

	return pusher_up;
}


auto Particles_builder::choose_interpolation(const vector<string> description,
	const Parameters& parameters, Fields& fields)
{
	/*
	*	тут может возникнуть сложность с одновременным использованием
	*	двух интерполяций, нужно будет это поправить.
	*/

	std::cout << "\t\t\tSetting interpolation... ";	
	std::unique_ptr<Interpolation> interpolation_up;

	size_t pos = string::npos;
	const auto& setting = description[PCONF::interpolation];

	if ((pos = setting.find("Boris_pusher:")) != string::npos)
	{
		if ( setting.find("+Interpolation", pos) != string::npos )
		{
			interpolation_up = make_unique<Boris_interpolation>(parameters, fields.E(), fields.B());
			std::cout << "done" << std::endl;
		}
		else {
			std::cout << "\n\t\t\t\twhat():  Known pusher not command" << std::endl;	
		}
	}
	else if ((pos = setting.find("Concrete_point_interpolation")) != string::npos )
	{
		if ((pos = setting.find("Homogenius_field", pos)) != string::npos)
		{
			pos = setting.find("E0=", pos);
			pos += 3;

			int divider_1 = setting.find(',', pos);
			int divider_2 = setting.find(',', divider_1+1);
			int end 	  = setting.find(')', divider_2+1);

			double E0x = stod(setting.substr(pos+1, divider_1-(pos+1)));
			double E0y = stod(setting.substr(divider_1+1, divider_2-(divider_1+1)));
			double E0z = stod(setting.substr(divider_2+1, end-(divider_2+1)));
			
			vector3 E0 = {E0x, E0y, E0z};

			pos = setting.find("B0=", pos);
			pos += 3;

			divider_1 = setting.find(',', pos);
			divider_2 = setting.find(',', divider_1+1);
			end 	  = setting.find(')', divider_2+1);

			double B0x = stod(setting.substr(pos+1, divider_1-(pos+1)));
			double B0y = stod(setting.substr(divider_1+1, divider_2-(divider_1+1)));
			double B0z = stod(setting.substr(divider_2+1, end-(divider_2+1)));
			
			vector3 B0 = {B0x, B0y, B0z};

			std::unique_ptr<Local_field_adder> adder = make_unique<Homogenius_field_adder>(E0, B0);

			interpolation_up = make_unique<Concrete_point_interpolation>(std::move(adder));
			std::cout << "done" << std::endl;
		}
		else {
			std::cout << "\n\t\t\t\twhat():  Known interpolation not adder" << std::endl;
		}
	}
	else {
		std::cout << "\n\t\t\t\twhat():  Initialization error: No matching interpolation" << std::endl;	
	}

	return interpolation_up;
}


auto Particles_builder::choose_decomposition(const vector<string> description,
	const Parameters& parameters, Fields& fields)
{
	std::cout << "\t\t\tSetting decomposition... ";	
	std::unique_ptr<Decomposition> decomposition_up;

	const auto& setting = description[PCONF::decomposition]; 

	if ( setting.find("Esirkepov_density_decomposition") != string::npos ) {
		decomposition_up = make_unique<Esirkepov_density_decomposition>(
			parameters, fields.J());
		std::cout << "done" << std::endl;
	}
	else {
		std::cout << "\n\t\t\t\twhat():  Initialization error: No matching density decomposition" << std::endl;	
	}

	return decomposition_up;
}


std::map<string, std::unique_ptr<Particles>> Particles_builder::build(
	Fields& fields,
	std::list<Command_up>& settings_before_main_cycle)
{
	std::map<string, Particles_up> list_of_particles;
	
#if there_are_particles
	std::cout << "\tBulding particles:" << std::endl;

	std::map<string, string> loaded_after;
	for (const auto& [name, descrtiption] : species)
	{
		std::cout << "\t\tBuilding \"" << name << "\"" << std::endl;

		auto parameters = this->config_parameters(descrtiption[0]); 

		Particles_up particles = make_unique<Particles>(
			std::move(parameters),
			std::move(this->choose_pusher(descrtiption[0], parameters, fields)),
			std::move(this->choose_interpolation(descrtiption[0], parameters, fields)),
			std::move(this->choose_decomposition(descrtiption[0], parameters, fields)),
			std::move(this->x_boundary()),
			std::move(this->y_boundary()),
			std::move(this->diagnostics_list(name, descrtiption[1])));

		try
		{
			load_particles(particles.get(), descrtiption[0], particles->get_parameters(), settings_before_main_cycle);
			std::cout << "\n\t\t\tdone" << std::endl;
		}
		catch (const string& loading_setting)
		{
			loaded_after[name] = loading_setting;
			std::cout << "\n\t\t\tdone" << std::endl;
		}
		catch (...) {
			std::cout << "\n\t\t\tCatched an unexpected exception"
				<< "while trying to load particles!" << std::endl;
		}

		std::cout << "\t\tdone (" << particles.get() << ")\n" << std::endl;		
		list_of_particles[name] = std::move(particles);
	}

	for (const auto& [name, loading_setting] : loaded_after)
	{
		Particles *const named = list_of_particles[name].get();

		if (loading_setting.starts_with("copy_coordinates"))
		{
			size_t pos = string("copy_coordinates_from_").size();
			
			size_t divider = loading_setting.find(',', pos+1);
			
			string copy_from = loading_setting.substr(pos, divider-pos);
			
			string configuration  = loading_setting.substr(divider+1);

			auto [_1, _2, load_impulse] = get_configuration_functions(configuration);

			settings_before_main_cycle.push_back(make_unique<Copy_coordinates>(
				named, list_of_particles[copy_from].get(), load_impulse));
		}
		else if (loading_setting.starts_with("ionize_particles"))
		{}
		else
		{
			std::cout << "\t\tThere is no known loading setting" << std::endl;
		}
	}

	std::cout << "\tdone!\n" << std::endl;
#endif
	
	return list_of_particles;
}


// Определение требуемых диагностик ---------------------------------------------------------------

std::vector<std::unique_ptr<Particles_diagnostic>> Particles_builder::diagnostics_list(
	string name_of_sort, vector<string> particle_diagnostics)
{
	std::cout << "\t\t\tSetting diagnostics...";
	// Возвращает список необходимых диагностик для частиц
	std::vector<diagnostic_up> vec_diagnostics{};
	
	#if particles_are_diagnosed
		if ( particle_diagnostics.empty() ) {
			std::cout << "what():  Initialization error: particles are diagnosed "
					<< "but no particle diagnostics in file [./constants.h]" << std::endl;
		}
		else {
			for (const auto& now : particle_diagnostics) {
				std::cout << "\n\t\t\t\t" << now;

				if ( now == "density" ) {
					vec_diagnostics.emplace_back( 
						make_unique<distribution_moment>(dir_name + "/" + name_of_sort,
							1000 * dx, 2000 * dx, dx, // (SIZE_X*dx/2 - 1.3*r_prop*r_larm)
							1000 * dy, 2000 * dy, dy, // (SIZE_Y*dy/2 - 1.3*r_prop*r_larm)
							make_unique<XY_projector>(), make_unique<zeroth_moment>()));
				}
				else if ( now == "1_of_VxVy" ) {
					vec_diagnostics.emplace_back(
						make_unique<distribution_moment>(dir_name + "/" + name_of_sort,
							-1.,	+1.,	0.01,
							-1.,	+1.,	0.01,
							make_unique<VxVy_projector>(), make_unique<zeroth_moment>()));
				}
				else if ( now == "first_Vx_moment" ) {
					vec_diagnostics.emplace_back( 
						make_unique<distribution_moment>(dir_name + "/" + name_of_sort,
							1000 * dx, 2000 * dx, dx, // (SIZE_X*dx/2 - 1.3*r_prop*r_larm), (SIZE_X*dx/2 + 1.3*r_prop*r_larm), dx,
							1000 * dy, 2000 * dy, dy, // (SIZE_Y*dy/2 - 1.3*r_prop*r_larm), (SIZE_Y*dy/2 + 1.3*r_prop*r_larm), dy,
							make_unique<XY_projector>(), make_unique<first_Vx_moment>()));
				}
				else if ( now == "first_Vy_moment" ) {
					vec_diagnostics.emplace_back( 
						make_unique<distribution_moment>(dir_name + "/" + name_of_sort,
							1000 * dx, 2000 * dx, dx, // (SIZE_X*dx/2 - 1.3*r_prop*r_larm), (SIZE_X*dx/2 + 1.3*r_prop*r_larm), dx,
							1000 * dy, 2000 * dy, dy, // (SIZE_Y*dy/2 - 1.3*r_prop*r_larm), (SIZE_Y*dy/2 + 1.3*r_prop*r_larm), dy,
							make_unique<XY_projector>(), make_unique<first_Vy_moment>()));
				}
				else if ( now == "first_Vr_moment" ) {
					vec_diagnostics.emplace_back( 
						make_unique<distribution_moment>(dir_name + "/" + name_of_sort,
							1000 * dx, 2000 * dx, dx, // (SIZE_X*dx/2 - 1.3*r_prop*r_larm), (SIZE_X*dx/2 + 1.3*r_prop*r_larm), dx,
							1000 * dy, 2000 * dy, dy, // (SIZE_Y*dy/2 - 1.3*r_prop*r_larm), (SIZE_Y*dy/2 + 1.3*r_prop*r_larm), dy,
							make_unique<XY_projector>(), make_unique<first_Vr_moment>()));
				}
				else if ( now == "first_Vphi_moment" ) {
					vec_diagnostics.emplace_back( 
						make_unique<distribution_moment>(dir_name + "/" + name_of_sort,
							1000 * dx, 2000 * dx, dx, // (SIZE_X*dx/2 - 1.3*r_prop*r_larm), (SIZE_X*dx/2 + 1.3*r_prop*r_larm), dx,
							1000 * dy, 2000 * dy, dy, // (SIZE_Y*dy/2 - 1.3*r_prop*r_larm), (SIZE_Y*dy/2 + 1.3*r_prop*r_larm), dy,
							make_unique<XY_projector>(), make_unique<first_Vphi_moment>()));
				}
				// else if ( now == "chosen_particles" ) {
				// 	vec_diagnostics.emplace_back( 
				// 		make_unique<chosen_particles>(dir_name + "/" + name_of_sort + "/" + now, choose_indexes()));
				// }
				else if ( now == "energy" ) {
					vec_diagnostics.emplace_back(
						make_unique<particles_energy>(dir_name + "/" + name_of_sort + "/" + now));
				}
				else {
					std::cout << "what():  Initialization error: No matching diagnostics for particles." << std::endl;
				}
	 		}
	 	}
	#endif	
	std::cout << "\n\t\t\tdone" << std::endl;
	
	vec_diagnostics.shrink_to_fit();
	return vec_diagnostics;
}