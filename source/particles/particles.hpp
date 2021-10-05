#ifndef MANAGERS_PARTICLES_HPP
#define MANAGERS_PARTICLES_HPP

//#################################################################################################

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <forward_list>

// #include "./add_ionization.hpp"
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
		Particle_parameters&, std::vector<Point>&&,
		std::function<void(Point&, double)>&& x_boundary,
		std::function<void(Point&, double)>&& y_boundary,
		std::forward_list<diagnostic_up>&&	);
	void set_push_commands(std::forward_list<pcommand_up>&&);
	Particle_parameters& get_parameters() { return parameters_; }

	// main Particles methods
	void push();
	void diagnose(int t);
	
	friend class Ionization;

private:
	// main Kinetic_particles fields
	Particle_parameters parameters_;
	std::vector<Point> points_;

	// solvers for particles
	std::forward_list<pcommand_up> push_commands_;
	
	void boundaries_processing(Point&, double size_x, double size_y);
	std::function<void(Point&, double)> x_boundary_ = nullptr;
	std::function<void(Point&, double)> y_boundary_ = nullptr;	

	// list of diagnostics for Particles
	std::forward_list<diagnostic_up> diagnostics_;
};


//#################################################################################################

#endif // MANAGERS_PARTICLES_HPP
