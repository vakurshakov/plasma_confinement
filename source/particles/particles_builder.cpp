#include "particles_builder.hpp"

#include <cmath>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <iostream>
#include <functional>

#include "./particles.hpp"
#include "./particles_load.hpp"
#include "./particle/point.hpp"
#include "../diagnostics/density.hpp"
#include "../diagnostics/energy.hpp"
#include "../diagnostics/phase_diagram.hpp"
#include "../fields/fields.hpp"
#include "../solvers/Boris_pusher.hpp"
#include "../solvers/Esirkepov_density_decomposition.hpp"
#include "../constants.h"

using std::vector, std::string, std::make_unique, std::stod, std::stoi;
using diagnostic_up = std::unique_ptr<Diagnostic>;
using Particles_up = std::unique_ptr<Particles>;

enum PCONF { pusher, decomposition,
			sort_charge, sort_mass, sort_density, number_of_particles_in_cell,
			configuration, cell_filling,
			temperature_x, temperature_y, temperature_z,
			initial_impulse };


Particle_parameters Particles_builder::config_parameters(const vector<string> parameters)
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
	std::cout << "\n\t\t\tdone (" << &params << ")" << std::endl;
	return params;
}


vector<Point> Particles_builder::load_particles(
	const vector<string> distribution, const Particle_parameters& parameters)
{
	std::cout << "\t\t\tLoading points...";
	string configuration =	distribution[PCONF::configuration];
	std::function<bool(int cell_number_nx, int cell_number_ny)> this_is_config_cell = nullptr;
	std::function<void(double x, double y,
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
	else if (configuration == "two_particles") {
		// Так можно проверить race-condition декомпозиции. 
		this_is_config_cell = [](int x, int y) -> bool {
			return 	(x == (SIZE_X/2 - 1) && y == SIZE_Y/2) ||
					(x == (SIZE_X/2 + 1) && y == SIZE_Y/2);
		};

		load_impulse = [](double x, double y,
			double mass, double Tx, double Ty, double Tz,
			double p0, double* px, double* py, double* pz) {
				*px = 0;
				*py = 1e-1;
				*pz = 0;
		};	
	}
  

	string cell_filling = distribution[PCONF::cell_filling];
	std::function<void(int sequential_number, int Np,
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
					points.emplace_back(Point({x, y}, {px, py, pz}));
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
	const Particle_parameters& parameters)
{
	std::cout << "\t\t\tSetting pusher..." << std::endl;	
	std::unique_ptr<Pusher> pusher_up;

	if (description[PCONF::pusher].find("Boris_pusher:") == 0) {
		if ( description[PCONF::pusher].find("+Push_particle") != string::npos ) {
			pusher_up = make_unique<Boris_pusher>(parameters);
		}
		else {
			std::cout << "\t\t\t\twhat():  Known pusher not command" << std::endl;	
		}
	}
	else {
		std::cout << "\t\t\t\twhat():  Initialization error: No matching Particle pusher" << std::endl;	
	}

	std::cout << "\t\t\tdone" << std::endl;
	return pusher_up;
}

auto Particles_builder::choose_interpolation(const vector<string> description,
	const Particle_parameters& parameters)
{
	std::cout << "\t\t\tSetting interpolation..." << std::endl;	
	std::unique_ptr<Interpolation> interpolation_up;

	if (description[PCONF::pusher].find("Boris_pusher:") == 0) {
		if ( description[PCONF::pusher].find("+Interpolation") != string::npos ) {
			interpolation_up = make_unique<Boris_interpolation>(parameters, fields_.E(), fields_.B());
		}
		else {
			std::cout << "\t\t\t\twhat():  Known pusher not command" << std::endl;	
		}
	}
	else {
		std::cout << "\t\t\t\twhat():  Initialization error: No matching particle pusher" << std::endl;	
	}

	std::cout << "\t\t\tdone" << std::endl;
	return interpolation_up;
}


auto Particles_builder::choose_decomposition(const vector<string> description,
	const Particle_parameters& parameters)
{
	std::cout << "\t\t\tSetting decomposition..." << std::endl;	
	std::unique_ptr<Decomposition> decomposition_up;

	// Декомпозиция токов
	if ( description[PCONF::decomposition] == "Esirkepov_density_decomposition" ) {
		decomposition_up = make_unique<Esirkepov_density_decomposition>(
			parameters, fields_.J());
	}
	else {
		std::cout << "\t\t\t\twhat():  Initialization error: No matching density decomposition" << std::endl;	
	}

	std::cout << "\t\t\tdone" << std::endl;
	return decomposition_up;
}

std::map<string, Particles_up> Particles_builder::build()
{
	std::map<string, Particles_up> list_of_particles;
	
	#if there_are_particles
	std::cout << "\tBulding particles:" << std::endl;

	srand(42);
	for (auto& description : species) {
		std::cout << "\t\tBuilding \"" << description.first << "\"" << std::endl;

		Particle_parameters parameters = this->config_parameters(description.second[0]); 

		Particles_up particles = make_unique<Particles>(
			parameters,
			std::move(this->load_particles(description.second[0], parameters)),
			std::move(this->choose_pusher(description.second[0], parameters)),
			std::move(this->choose_interpolation(description.second[0], parameters)),
			std::move(this->choose_decomposition(description.second[0], parameters)),
			std::move(this->x_boundary()),
			std::move(this->y_boundary()),
			std::move(this->diagnostics_list(description.first,
				description.second[1])) );

		std::cout << "\t\tdone (" << particles.get() << ")\n" << std::endl;		
		list_of_particles[description.first] = std::move(particles);
	}

	std::cout << "\tdone!\n" << std::endl;
	#endif
	
	return list_of_particles;
}


// Определение требуемых диагностик ---------------------------------------------------------------

std::vector<diagnostic_up> Particles_builder::diagnostics_list(
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
			for (auto& now : particle_diagnostics) {
				if ( now == "energy" ) {
					std::cout << "\n\t\t\t\t" << now;
					vec_diagnostics.emplace_back(make_unique<particles_energy>(dir_name + "/" + name_of_sort + "/" + now));
				}
				else if ( now == "diagram_vx_on_y" ) {
					std::cout << "\n\t\t\t\t" << now;
					vec_diagnostics.emplace_back(make_unique<diagram_vx_on_y>(dir_name + "/" + name_of_sort + "/" + now,
						-1., 1., 0.01, 0, SIZE_Y));
				}
				else if ( now == "density" ) {
					std::cout << "\n\t\t\t\t" << now;
					vec_diagnostics.emplace_back( make_unique<density>(dir_name + "/" + name_of_sort + "/" + now) );
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