#include "./particles_builder.hpp"

#include <cmath>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <functional>
#include <forward_list>

#include "./fields.hpp"
#include "./particles.hpp"
#include "./particles_load.hpp"
#include "../command/command.hpp"
#include "../command/cmd_particle_push.hpp"
#include "../command/cmd_density_decomposition.hpp"
#include "../diagnostics/density.hpp"
#include "../diagnostics/energy.hpp"
#include "../diagnostics/phase_diagram.hpp"
#include "../particle/point.hpp"
#include "../solvers/solvers.hpp"
#include "../constants.h"


using std::vector, std::string, std::forward_list, std::make_unique, std::stod, std::stoi;
using v3f = vector3_field;
using v3f_up = std::unique_ptr<vector3_field>;
using diagnostic_up = std::unique_ptr<Diagnostic>;
using pcommand_up = std::unique_ptr<Particle_command>;


enum PCONF { pusher, decomposition,
			sort_charge, sort_mass, sort_density, number_of_particles_in_cell,
			configuration, cell_filling,
			temperature_x, temperature_y, temperature_z,
			initial_impulse };

Particle_parameters Particles_builder::config_parameters(const std::vector<string> parameters)
{
	Particle_parameters params(	stod(parameters[PCONF::sort_charge]),
								stod(parameters[PCONF::sort_mass]),
								stod(parameters[PCONF::sort_density]),
								stoi(parameters[PCONF::number_of_particles_in_cell])	);
	return params;
}

vector<Point> Particles_builder::load_particles(const std::vector<string> distribution)
{
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

	double Tx = stod(distribution[PCONF::temperature_x]); 
	double Ty = stod(distribution[PCONF::temperature_y]); 
	double Tz = stod(distribution[PCONF::temperature_z]);  
	double p0 =	stod(distribution[PCONF::initial_impulse]);

	// Вот это, конечно, нужно по-другому сделать
	double mass = stod(distribution[PCONF::sort_mass]);
	int Np = stoi(distribution[PCONF::number_of_particles_in_cell]);

	// Будем задавать рандом одинаковым числом для
	// воспроизводимости экспериментов  
	srand(42);
	vector<Point> points;

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
					points.push_back(temp);
				}
			}
		}
	}}	

	points.shrink_to_fit();
	return points;
}

auto Particles_builder::set_pusher(const std::vector<string> description) {
	Boris_pusher* pusher;

	if (description[PCONF::pusher].find("Boris_pusher:")) {
		pusher = new Boris_pusher();
	}
	else {
		std::cout << "what():  Initialization error: No matching Particle pusher" << std::endl;	
	}

	return pusher;
}

auto Particles_builder::x_boundary()
{	
	std::function<void(Point*, double)> x_boundary = nullptr;
	
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
	std::function<void(Point*, double)> y_boundary = nullptr;
	
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
	Boris_pusher* pusher,
	const Particle_parameters& parameters)
{
	// Возвращает список пуш-команд [ работает с файлом ./constants.h ]
	forward_list<pcommand_up> commands;

	// Толкатель
	if ( description[PCONF::pusher].find("+Interpolation") ) {
		commands.push_front(make_unique<Interpolate_fields>(pusher, fields_->E(), fields_->B(), parameters));
	}

	if ( description[PCONF::pusher].find("+Push_particle") ) {
		commands.push_front(make_unique<Push_particle>(pusher, parameters));
	}

	// Декомпозиция токов
	if ( description[PCONF::decomposition] == "Esirkepov_density_decomposition" ) {
		commands.push_front(make_unique<Density_decomposition>(Esirkepov_density_decomposition, 
			parameters, &fields_->J()));
	}

	return commands;
}

forward_list<Particles> Particles_builder::build()
{
	forward_list<Particles> list_of_particles;
	#if there_are_particles		
	for (auto description : species) {
		auto parameters = this->config_parameters(description.second[0]);
		auto pusher = this->set_pusher(description.second[0]);
		Particles particles( parameters,
							 this->load_particles(description.second[0]),
							 pusher,
							 this->x_boundary(),
							 this->y_boundary(),
							 this->diagnostics_list(description.first,
							 	description.second[1]) );

		particles.set_push_commands(this->particle_push_commands(description.second[0],
			pusher, parameters)); 
		list_of_particles.push_front(std::move(particles));
	}

	#endif
	return list_of_particles;	
}


// Определение требуемых диагностик ---------------------------------------------------------------

forward_list<diagnostic_up> Particles_builder::diagnostics_list(
	string name_of_sort, vector<string> particle_diagnostics)
{
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
					fl_diagnostics.push_front(make_unique<particles_energy>(dir_name + "/" + name_of_sort + "/" + now));
				}
				else if ( now == "diagram_vx_on_y" ) {
					fl_diagnostics.push_front(make_unique<diagram_vx_on_y>(dir_name + "/" + name_of_sort + "/" + now,
						-1., 1., 0.01, 0, SIZE_Y));
				}
				else if ( now == "density" ) {
					fl_diagnostics.push_front( make_unique<density>(dir_name + "/" + name_of_sort + "/" + now) );
				}
				else {
					std::cout << "what():  Initialization error: No matching diagnostics for particles." << std::endl;
				}
	 		}
	 	}
	#endif	
	return fl_diagnostics;
}