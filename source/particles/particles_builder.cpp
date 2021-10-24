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
#include "../diagnostics/chosen_particles.hpp"
#include "../fields/fields.hpp"
#include "../solvers/Boris_pusher.hpp"
#include "../solvers/Esirkepov_density_decomposition.hpp"
#include "../solvers/concrete_point_interpolation.hpp"
#include "../constants.h"

using std::vector, std::string, std::make_unique, std::stod, std::stoi;
using Particles_up = std::unique_ptr<Particles>;

enum PCONF { push, interpolation, decomposition,
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
	std::function<int(int Np)> get_number_of_particles = nullptr;
	std::function<void(double x, double y,
		double mass, double Tx, double Ty, double Tz,
		double p0, double* px, double* py, double* pz)> load_impulse = nullptr; 
	
	if (configuration == "ring") {
		this_is_config_cell = cell_on_a_ring;
		get_number_of_particles = get_number_of_particles_on_ring;
		load_impulse = load_annular_impulse;

	}
	else if (configuration == "circle") {
		this_is_config_cell = cell_in_a_circle;
		get_number_of_particles = get_number_of_particles_in_circle;
		load_impulse = load_uniform_impulse;
	}
	else if (configuration == "single_particle") {
		// Решение для тестов, при этом лучше положить Np = 1.
		this_is_config_cell = [](int x, int y) -> bool {
			return x == SIZE_X/2 && y == SIZE_Y/2;
		};

		load_impulse = [](double x, double y,
			double mass, double Tx, double Ty, double Tz,
			double p0, double* px, double* py, double* pz) -> void {
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
	points.reserve( get_number_of_particles(Np) );
	std::cout << points.capacity();

	std::cout << "\n\t\t\t\tConfiguration: " << configuration
		<< ", way of filling cell: " << cell_filling << ";";
	if (cell_filling != "delayed") {

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
	std::cout << "\t\t\tSetting pusher... ";	
	std::unique_ptr<Pusher> pusher_up;

	const auto& setting = description[PCONF::push];

	if (setting.find("Boris_pusher:") != string::npos) {
		if ( setting.find("+Push_particle") != string::npos ) {
			pusher_up = make_unique<Boris_pusher>(parameters);
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
	const Particle_parameters& parameters)
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
		if ( setting.find("+Interpolation", pos) != string::npos ) {
			interpolation_up = make_unique<Boris_interpolation>(parameters, fields_.E(), fields_.B());
			std::cout << "done" << std::endl;
		}
		else {
			std::cout << "\n\t\t\t\twhat():  Known pusher not command" << std::endl;	
		}
	}
	else if ((pos = setting.find("Concrete_point_interpolation")) != string::npos )
	{
		if ((pos = setting.find("Homogenius_field", pos)) != string::npos) {
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
	const Particle_parameters& parameters)
{
	std::cout << "\t\t\tSetting decomposition... ";	
	std::unique_ptr<Decomposition> decomposition_up;

	const auto& setting = description[PCONF::decomposition]; 

	if ( setting.find("Esirkepov_density_decomposition") != string::npos ) {
		decomposition_up = make_unique<Esirkepov_density_decomposition>(
			parameters, fields_.J());
		std::cout << "done" << std::endl;
	}
	else {
		std::cout << "\n\t\t\t\twhat():  Initialization error: No matching density decomposition" << std::endl;	
	}

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
		std::vector<Point> points = this->load_particles(description.second[0], parameters); 

		Particles_up particles = make_unique<Particles>(
			parameters,
			std::move(points),
			std::move(this->choose_pusher(description.second[0], parameters)),
			std::move(this->choose_interpolation(description.second[0], parameters)),
			std::move(this->choose_decomposition(description.second[0], parameters)),
			std::move(this->x_boundary()),
			std::move(this->y_boundary()),
			std::move(this->diagnostics_list(description.first,
				description.second[1], points)) );

		std::cout << "\t\tdone (" << particles.get() << ")\n" << std::endl;		
		list_of_particles[description.first] = std::move(particles);
	}

	std::cout << "\tdone!\n" << std::endl;
	#endif
	
	return list_of_particles;
}


// Определение требуемых диагностик ---------------------------------------------------------------

std::vector<std::unique_ptr<Particles_diagnostic>> Particles_builder::diagnostics_list(
	string name_of_sort, vector<string> particle_diagnostics, const std::vector<Point>& points)
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
						make_unique<density>(dir_name + "/" + name_of_sort + "/" + now) );
				}
				else if ( now == "chosen_particles" ) {
					vec_diagnostics.emplace_back( 
						make_unique<chosen_particles>(dir_name + "/" + name_of_sort + "/" + now, way_to_choose(points)));
				}
				else if ( now == "diagram_vx_on_y" ) {
					vec_diagnostics.emplace_back(
						make_unique<diagram_vx_on_y>(dir_name + "/" + name_of_sort + "/" + now));
				}
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