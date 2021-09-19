#include "./entities_builder.hpp"

#include <map>
#include <cmath>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <filesystem>
#include <functional>

#include "./fields.hpp"
#include "./particles.hpp"
#include "./particles_load.hpp"
#include "../particle/point.hpp"
#include "../solvers/solvers.hpp"
#include "../constants.h"

// list of diagnostics
#include "../diagnostics/density.hpp"
#include "../diagnostics/energy.hpp"
#include "../diagnostics/field_along_the_line.hpp"
#include "../diagnostics/field_at_point.hpp"
#include "../diagnostics/phase_diagram.hpp"
#include "../diagnostics/whole_field.hpp"


using std::vector, std::map, std::multimap, std::string;
namespace fs = std::filesystem;


void Entities_builder::save_config()
{
	// Сохранение параметров (/*from*/ constants.h, /*to*/ dir_name)
	try{
		fs::copy("./dl_transverse_section/constants.h", dir_name + string("/constants.h"));
	}
	catch(fs::filesystem_error const& ex) {
		switch(ex.code().value()) 
		{
			case 2: // "No such file or directory"
				fs::create_directories(dir_name);
				fs::copy("./dl_transverse_section/constants.h", dir_name + string("/constants.h"));
				break;
			case 17: // "File exists"
				break;
			default:
				std::cout
					<< "what():  " << ex.what() << '\n'
					<< "path1(): " << ex.path1() << '\n'
					<< "path2(): " << ex.path2() << '\n'
					<< "code().value():    " << ex.code().value() << '\n'
					<< "code().message():  " << ex.code().message() << '\n'
					<< "code().category(): " << ex.code().category().name() << std::endl;
				break;
		}
	}
}

// Построение полей -------------------------------------------------------------------------------

function<void(v3f& E, v3f& B, v3f& j)> Entities_builder::propogator()
{
	// Вовзращает выбранный солвер (пропогатор) [ работает с файлом ./constants.h ]
	if ( field_solver.empty() ) {
		std::cout << "what():  Initialization error: No field_solver in file [./constants.h]" << std::endl; 
 	}
	else if ( field_solver == "FDTD_2D" ) {
		return FDTD_2D;			
	}
	else {
		std::cout << "what():  Initialization error: No matching field_solver" << std::endl;
	}
}


v3f_up Entities_builder::load_field(string type)
{
	// Возвращает сконфигурированные поля [ работает с файлом ./constants.h ]
	if ( boundaries.empty() ) {
		std::cout << "load_field:  Initialization error: No boundaries in file [./constants.h]" << std::endl;
	}
	else if ( boundaries == "px_py" ) {
		return make_unique<px_py_vector3_field>(SIZE_X, SIZE_Y);
	}
	else if ( boundaries == "rx_ry" ) {
		if ( type == "Electric" ) {
			return make_unique<rx_ry_Electric_field>(SIZE_X, SIZE_Y);
		}
		else if ( type == "Magnetic" ) {
			return make_unique<rx_ry_Magnetic_field>(SIZE_X, SIZE_Y);
		}
	}
	else if ( boundaries == "rx_py" ) {
		if ( type == "Electric" ) {
			return make_unique<rx_py_Electric_field>(SIZE_X, SIZE_Y);
		}
		else if ( type == "Magnetic" ) {
			return make_unique<rx_py_Magnetic_field>(SIZE_X, SIZE_Y);
		}
	}
	else if ( boundaries == "px_ry" ) {
		if ( type == "Electric" ) {
			return make_unique<px_ry_Electric_field>(SIZE_X, SIZE_Y);
		}
		else if ( type == "Magnetic" ) {
			return make_unique<px_ry_Magnetic_field>(SIZE_X, SIZE_Y);
		}
	}
	else {
		std::cout << "load_field:  Initialization error: No matching boundaries" << std::endl;	
	}
}

Fields Entities_builder::build_fields()
{
	#if there_are_fields
		Fields fields(	this->load_field("Electric"),
						this->load_field("Magnetic"),
						this->load_field("Electric"),
						this->propogator(),
						this->diagnostics_list(fields_diagnostics)	);
		return fields;
	#else
		Fields fields()
		return fields;
	#endif
}

// Построение частиц ------------------------------------------------------------------------------

enum PCONF { pusher, decomposition,
			charge, mass, density, number_of_particles_in_cell,
			configuration, cell_filling,
			temperature_x, temperature_y, temperature_z,
			initial_impulse };

Particle_parameters Entities_builder::config_parameters(std::vector<string> parameters)
{
	Particle_parameters params(	stod(parameters[PCONF::charge]),
								stod(parameters[PCONF::mass]),
								stod(parameters[PCONF::density]),
								stoi(parameters[PCONF::number_of_particles_in_cell])	);
	return params;
}

vector<Point> Entities_builder::load_particles(std::vector<string> distribution)
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
	double mass = stod(distribution[PCONF::mass]);
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

				if (isinf(px) || isinf(py) || isinf(pz)) { 
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

auto Entities_builder::particle_push(std::vector<string> description)
{
	// Возвращает необходимый толкатель [ работает с файлом ./constants.h ]
	std::function<void(const Particle_parameters&, Point&,
		const v3f& E, const v3f& B)> particle_push = nullptr;
	
	if ( description[PCONF::pusher].empty() ) {
		std::cout << "what():  Initialization error: No particle_push in file [./constants.h]" << std::endl;
	}
	else if ( description[PCONF::pusher] == "Boris_pusher" ) {
		particle_push = Boris_pusher;
	}
	else {
		std::cout << "what():  Initialization error: No matching particle_push" << std::endl;
	}
	return particle_push;
}

auto Entities_builder::density_decomposition(std::vector<string> description)
{
	// Возвращает необходимую декомпозицию токов [ работает с файлом ./constants.h ]
	std::function<void(const Particle_parameters&, const Point&,
		const vector2& r0, v3f& j)> density_decomposition = nullptr;
	
	if ( description[PCONF::decomposition].empty() ) {
		std::cout << "what():  Initialization error: No density_decomposition in file [./constants.h]" << std::endl;
	}
	else if ( description[PCONF::decomposition] == "Esirkepov_density_decomposition" ) {
		density_decomposition = Esirkepov_density_decomposition;
	}
	else {
		std::cout << "what():  Initialization error: No matching density_decomposition" << std::endl;
	}
	return density_decomposition;
}

auto Entities_builder::x_boundary()
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

auto Entities_builder::y_boundary()
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

map<string, Particles> Entities_builder::build_particles()
{
	map<string, Particles> particles;
	#if there_are_particles		

		for (auto description : species) {
			Particles *pts;
		
			if ( (description.first).find("Electrons") ) {		
				pts = new Electrons(	this->config_parameters(description.second[0]),
										this->load_particles(description.second[0]),
										this->particle_push(description.second[0]),
										this->density_decomposition(description.second[0]),
										this->x_boundary(),
										this->y_boundary(),
										this->diagnostics_list(description.first,
												description.second[1]) );
			}
			else if ( (description.first).find("Ions") ) {
				pts = new Ions(	this->config_parameters(description.second[0]),
								this->load_particles(description.second[0]),
								this->particle_push(description.second[0]),
								this->density_decomposition(description.second[0]),
								this->x_boundary(),
								this->y_boundary(),
								this->diagnostics_list(description.first,
										description.second[1]) );
			}
			
			particles.emplace(description.first, move(*pts));
		}

	#endif
	return particles;	
}


// Определение требуемых диагностик ---------------------------------------------------------------

enum FD_DESC { FIELD, AXIS, PX, PY };

forward_list<diagnostic_up> Entities_builder::diagnostics_list(
	multimap<string, vector<string>> fields_diagnostics)
{
	// Возвращает список необходимых диагностик для полей
	forward_list<diagnostic_up> fl_diagnostics{};
	
	#if fields_are_diagnosed
		if ( fields_diagnostics.empty() ) {
			std::cout << "what():  Initialization error: fields are diagnosed "
					<< "but no fields_diagnostics in file [./constants.h]" << std::endl;
		}
		else {	
			for (auto& now : fields_diagnostics) {
				if ( now.first == "energy" ) {
					fl_diagnostics.push_front(make_unique<fields_energy>(dir_name + "/" + now.first));
				}
				else if ( now.first == "whole_field" ) {
					fl_diagnostics.push_front(make_unique<whole_field>(dir_name + "/fields/" + now.first,
						now.second[FD_DESC::FIELD] + now.second[FD_DESC::AXIS],
						now.second[FD_DESC::FIELD], now.second[FD_DESC::AXIS] ) );
				}
				else if ( now.first == "field_along_x_axis" ) {
					fl_diagnostics.push_front(make_unique<field_along_x_axis>(dir_name + "/fields/" + now.first,
						now.second[FD_DESC::FIELD] + now.second[FD_DESC::AXIS],
						now.second[FD_DESC::FIELD], now.second[FD_DESC::AXIS],
						stoi(now.second[FD_DESC::PX]) ) );
				}
				else if ( now.first == "field_along_y_axis" ) {
					fl_diagnostics.push_front(make_unique<field_along_y_axis>(dir_name + "/fields/" + now.first,
						now.second[FD_DESC::FIELD] + now.second[FD_DESC::AXIS],
						now.second[FD_DESC::FIELD], now.second[FD_DESC::AXIS],
						stoi(now.second[FD_DESC::PX]) ) );
				} 
				else if ( now.first == "field_at_point" ) {
					fl_diagnostics.push_front(make_unique<field_at_point>(dir_name + "/fields/" + now.first,
						now.second[FD_DESC::FIELD] + now.second[FD_DESC::AXIS],
						now.second[FD_DESC::FIELD], now.second[FD_DESC::AXIS], 
						stoi(now.second[FD_DESC::PX]), stoi(now.second[FD_DESC::PY]) ) );
				}
				else {
					std::cout << "what():  Initialization error: No matching diagnostics for fields." << std::endl;
				}
			}
			return fl_diagnostics; 
		}
	#endif
}

forward_list<diagnostic_up> Entities_builder::diagnostics_list(
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
			for (auto& diagnostic : particle_diagnostics) {
				if ( diagnostic == "energy" ) {
					fl_diagnostics.push_front(make_unique<particles_energy>(dir_name + "/" + name_of_sort + "/" + diagnostic));
				}
				else if ( diagnostic == "diagram_vx_on_y" ) {
					fl_diagnostics.push_front(make_unique<diagram_vx_on_y>(dir_name + "/" + name_of_sort + "/" + diagnostic,
						-1., 1., 0.01, 0, SIZE_Y));
				}
				else if ( diagnostic == "density" ) {
					std::cout << "Hello!!!\n";
					/*
					fl_diagnostics.push_front(make_unique<density>(dir_name + "/" + name_of_sort + "/" + diagnostic));
					*/
				}
				else {
					std::cout << "what():  Initialization error: No matching diagnostics for particles." << std::endl;
				}
	 		}
	 		return fl_diagnostics;
	 	}
	#endif	
}