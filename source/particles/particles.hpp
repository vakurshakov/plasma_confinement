#ifndef MANAGERS_PARTICLES_HPP
#define MANAGERS_PARTICLES_HPP

//#################################################################################################

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <forward_list>

#include "./particle/particle_parameters.hpp"
#include "./particle/point.hpp"
#include "../command/command.hpp" 
#include "../diagnostics/diagnostics.hpp"
#include "../solvers/Boris_pusher.hpp"
#include "../vectors/vector3_field.hpp"
#include "../constants.h"


using diagnostic_up = std::unique_ptr<Diagnostic>;
using pcommand_up = std::unique_ptr<Particle_command>;
using Pusher_up = std::unique_ptr<Boris_pusher>;


class Particles {
public:
	Particles() = default;
	Particles(
		const std::string particles_name,
		Particle_parameters&, Pusher_up& pusher,
		std::vector<Point>&&,
		std::function<void(Point&, double)>&& x_boundary,
		std::function<void(Point&, double)>&& y_boundary,
		std::forward_list<diagnostic_up>&& diagnostics 	);	
	void set_push_commands(std::forward_list<pcommand_up>&&);
	std::string get_name() const { return particles_name_; }

	// main Particles methods
	void push();
	void diagnose(int t);
	
	friend void create_particles(
		Particles* ionized, Particles* lost,
		std::function<bool(vector2&)>& if_particles_are_configurated,
		std::function<double(double)>& density_shape,
		std::function<double(vector2&)>& probability );

protected:
	const std::string particles_name_;

	// main Kinetic_particles fields
	Particle_parameters parameters_;
	std::vector<Point> points_;

	// solvers for particles
	Pusher_up pusher_;
	std::forward_list<pcommand_up> push_commands_;
	
	void boundaries_processing(Point&, double size_x, double size_y);
	std::function<void(Point&, double)> x_boundary_ = nullptr;
	std::function<void(Point&, double)> y_boundary_ = nullptr;

	// list of diagnostics for Particles
	std::forward_list<diagnostic_up> diagnostics_;
};


//#################################################################################################

#endif // MANAGERS_PARTICLES_HPP
