#include "particles_builder.hpp"

#include <cmath>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <iostream>
#include <functional>
#include <forward_list>

#include "./particles.hpp"
#include "./particles_load.hpp"
#include "./particle/point.hpp"
#include "../command/command.hpp"
#include "../command/cmd_particle_push.hpp"
#include "../command/cmd_density_decomposition.hpp"
#include "../diagnostics/density.hpp"
#include "../diagnostics/energy.hpp"
#include "../diagnostics/phase_diagram.hpp"
#include "../fields/fields.hpp"
#include "../solvers/solvers.hpp"
#include "../constants.h"


using std::vector, std::string, std::forward_list,
	std::make_unique, std::make_shared, std::stod, std::stoi;
using diagnostic_up = std::unique_ptr<Diagnostic>;
using pcommand_up = std::unique_ptr<Particle_command>;
using Particles_up = std::unique_ptr<Particles>;
using Pusher_sp = std::shared_ptr<Boris_pusher>;


enum PCONF { pusher, decomposition,
			sort_charge, sort_mass, sort_density, number_of_particles_in_cell,
			configuration, cell_filling,
			temperature_x, temperature_y, temperature_z,
			initial_impulse };

Particle_parameters Particles_builder::config_parameters(const std::vector<string> parameters)
{
	std::cout << "\t\t\tSetting parameters...";
	Particle_parameters params(	stod(parameters[PCONF::sort_charge]),
								stod(parameters[PCONF::sort_mass]),
								stod(parameters[PCONF::sort_density]),
								stoi(parameters[PCONF::number_of_particles_in_cell]),
								stod(parameters[PCONF::temperature_x]), 
								stod(parameters[PCONF::temperature_y]), 
								stod(parameters[PCONF::temperature_z]),
								stod(parameters[PCONF::initial_impulse])
	);
	std::cout << "\n\t\t\t\t" << "e=" << parameters[PCONF::sort_charge] << ", "
							  << "m=" << parameters[PCONF::sort_mass] << ", "
							  << "n=" << parameters[PCONF::sort_density] << ", "
							  << "Np=" << parameters[PCONF::number_of_particles_in_cell] << ",\n\t\t\t\t"
							  << "Tx=" << parameters[PCONF::temperature_x] << ", "
							  << "Ty=" << parameters[PCONF::temperature_y] << ", "
							  << "Tz=" << parameters[PCONF::temperature_z] << ", "
							  << "p0=" << parameters[PCONF::initial_impulse] << ";";
	std::cout << "\n\t\t\tdone" << std::endl;
	return params;
}


vector<Point> Particles_builder::load_particles(
	const std::vector<string> distribution, const Particle_parameters& parameters)
{
	std::cout << "\t\t\tLoading points...";
	string configuration =	distribution[PCONF::configuration];
	function<bool(int cell_number_nx, int cell_number_ny)> this_is_config_cell = nullptr;
	function<void(double x, double y,
		double mass, double Tx, double Ty, double Tz,
		double p0, double* px, double* py, double* pz)> load_impulse = nullptr; 
	
	if (configuration == "ring") {
		this_is_config_cell = cell_on_a_ring;
		load_impulse = load_annular_impulse;
	}
	else if (configuration == "circle") {
		this_is_config_cell = cell_in_a_circle;
		load_impulse = load_uniform_impulse;
	}
	else if (configuration == "single_particle") {
		// Решение для тестов, при этом лучше положить Np = 1.
		this_is_config_cell = [](int x, int y) -> bool {
			return x == SIZE_X/2 && y == SIZE_Y/2;
		};

		load_impulse = [](double x, double y,
			double mass, double Tx, double Ty, double Tz,
			double p0, double* px, double* py, double* pz) {
				*px = 0;
				*py = 1e-3;
				*pz = 0;
		};
	}
 

	string cell_filling = distribution[PCONF::cell_filling];
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
    const double mass = parameters.m();
    const double Tx = parameters.Tx();
    const double Ty = parameters.Ty();
    const double Tz = parameters.Tz();
    const double p0 = parameters.p0();

	vector<Point> points;

	std::cout << "\n\t\t\t\tConfiguration: " << configuration
		<< ", way of filling cell: " << cell_filling << ";";
	if (!configuration.empty() && !cell_filling.empty()) {

	for (int nx = 0; nx < SIZE_X; ++nx) {
	for (int ny = 0; ny < SIZE_Y; ++ny) {
		
		if ( this_is_config_cell(nx, ny) ) {
			int err = 0;
			for (int i = 0; i < Np + err; ++i) {
				double x, y;
				fill_the_cell(i, Np, nx, ny, &x, &y);

				double px, py, pz;
				load_impulse(x, y, mass, Tx, Ty, Tz, p0, &px, &py, &pz);

				if (std::isinf(px) || std::isinf(py) || std::isinf(pz)) { 
					++err;
					continue;
				}
				else {
					Point temp({x, y}, {px, py, pz});
					points.push_back(std::move(temp));
				}
			}
		}
	}}	
		points.shrink_to_fit();
	}
	std::cout << "\n\t\t\t\t" << points.size() << " point(s) have been loaded";
	std::cout << "\n\t\t\tdone" << std::endl;
	return points;
}


auto Particles_builder::x_boundary()
{	
	std::function<void(Point&, double)> x_boundary = nullptr;
	
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
	std::function<void(Point&, double)> y_boundary = nullptr;
	
	const size_t left_boundary = 3; //px_py
	
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


auto Particles_builder::particle_push_commands(const std::vector<string> description,
	const Particle_parameters& parameters)
{
	// Возвращает список пуш-команд [ работает с файлом ./constants.h ]
	std::cout << "\t\t\tSetting particle push commands...";
	forward_list<pcommand_up> commands;

	// Толкатель (cейчас это пушер Бориса)
	std::cout << "\t\t\tSetting pusher...";	
	Pusher_sp pusher = nullptr;

	if (description[PCONF::pusher].find("Boris_pusher:") == 0) {
		pusher = make_shared<Boris_pusher>();

		if ( description[PCONF::pusher].find("+Interpolation") != string::npos ) {
			commands.push_front(make_unique<Interpolate_fields>(
				pusher, fields_.E(), fields_.B(), parameters));
			std::cout << "\n\t\t\t\t Interpolation: (&pusher=" << pusher.get() << ")"
				<< ", &E=" << &fields_.E() << ", &B=" << &fields_.B() << ")";
		}
		if ( description[PCONF::pusher].find("+Push_particle") != string::npos ) {
			commands.push_front(make_unique<Push_particle>(pusher, parameters));
			std::cout << "\n\t\t\t\t Push_particle: (&pusher=" << pusher.get() << ")";
		}
	}
	else {
		std::cout << "\t\t\twhat():  Initialization error: No matching Particle pusher" << std::endl;	
	}

	// Декомпозиция токов
	if ( description[PCONF::decomposition] == "Esirkepov_density_decomposition" ) {
		commands.push_front(make_unique<Density_decomposition>(
			Esirkepov_density_decomposition, parameters, fields_.J()));
		std::cout << "\n\t\t\t\t E.d.d.: " << commands.front().get()
			<< " (&J=" << &fields_.J() << ")";
	}

	std::cout << "\n\t\t\tdone" << std::endl;
	// Разворачиваем комманды, они в другом порядке
	commands.reverse();
	return commands;
}


map<string, Particles_up> Particles_builder::build()
{
	map<string, Particles_up> list_of_particles;
	
	#if there_are_particles
	std::cout << "\tBulding particles:" << std::endl;

	srand(42);
	for (auto& description : species) {
		std::cout << "\t\tBuilding \"" << description.first << "\"" << std::endl;

		Particle_parameters parameters = this->config_parameters(description.second[0]); 

		Particles_up particles = make_unique<Particles>(
			parameters,
			std::move(this->load_particles(description.second[0], parameters)),
			std::move(this->x_boundary()),
			std::move(this->y_boundary()),
			std::move(this->diagnostics_list(description.first,
				description.second[1])) );

		particles->set_push_commands(
			std::move(this->particle_push_commands (
				description.second[0],
				particles->get_parameters() )));

		std::cout << "\t\tdone (" << particles.get() << ")\n" << std::endl;		
		list_of_particles[description.first] = std::move(particles);
	}

	std::cout << "\tdone!\n" << std::endl;
	#endif
	
	return list_of_particles;
}


// Определение требуемых диагностик ---------------------------------------------------------------

forward_list<diagnostic_up> Particles_builder::diagnostics_list(
	string name_of_sort, vector<string> particle_diagnostics)
{
	std::cout << "\t\t\tSetting diagnostics...";
	// Возвращает список необходимых диагностик для частиц
	forward_list<diagnostic_up> fl_diagnostics{};
	
	#if particles_are_diagnosed
		if ( particle_diagnostics.empty() ) {
			std::cout << "what():  Initialization error: particles are diagnosed "
					<< "but no particle diagnostics in file [./constants.h]" << std::endl;
		}
		else {
			for (auto& now : particle_diagnostics) {
				if ( now == "energy" ) {
					std::cout << "\n\t\t\t\t" << now;
					fl_diagnostics.push_front(make_unique<particles_energy>(dir_name + "/" + name_of_sort + "/" + now));
				}
				else if ( now == "diagram_vx_on_y" ) {
					std::cout << "\n\t\t\t\t" << now;
					fl_diagnostics.push_front(make_unique<diagram_vx_on_y>(dir_name + "/" + name_of_sort + "/" + now,
						-1., 1., 0.01, 0, SIZE_Y));
				}
				else if ( now == "density" ) {
					std::cout << "\n\t\t\t\t" << now;
					fl_diagnostics.push_front( make_unique<density>(dir_name + "/" + name_of_sort + "/" + now) );
				}
				else {
					std::cout << "what():  Initialization error: No matching diagnostics for particles." << std::endl;
				}
	 		}
	 	}
	#endif	
	std::cout << "\n\t\t\tdone" << std::endl;
	return fl_diagnostics;
}