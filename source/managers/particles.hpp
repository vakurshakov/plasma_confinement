#ifndef MANAGERS_PARTICLES_HPP
#define MANAGERS_PARTICLES_HPP

//#################################################################################################

#include <memory>
#include <vector>
#include <functional>
#include <forward_list>

#include "../command/command.hpp" 
#include "../diagnostics/diagnostics.hpp"
#include "../particle/point.hpp"
#include "../particle/particle_parameters.hpp"
#include "../solvers/Boris_pusher.hpp"
#include "../vectors/vector3_field.hpp"
#include "../constants.h"


using v3f = vector3_field;
using diagnostic_up = std::unique_ptr<Diagnostic>;
using pcommand_up = std::unique_ptr<Particle_command>;


class Particles {
public:
	Particles() = default;
	Particles(
		Particle_parameters, std::vector<Point>&&,
		Boris_pusher* pusher,
		std::function<void(Point*, double)>&& x_boundary,
		std::function<void(Point*, double)>&& y_boundary,
		std::forward_list<diagnostic_up>&& diagnostics 	);
	
	~Particles() {
		delete pusher_;
	}


	// main Particles methods
	void push();
	void diagnose(int t);

	bool empty();
	void set_push_commands(std::forward_list<pcommand_up>&&);

protected:
	// main KineticParticles fields
	Particle_parameters parameters_;
	std::vector<Point> points_;

	// solvers for particles
	Boris_pusher* pusher_;
	std::forward_list<pcommand_up> push_commands_;
	
	void boundaries_processing(int i, double size_x, double size_y);
	std::function<void(Point*, double)> x_boundary_ = nullptr;
	std::function<void(Point*, double)> y_boundary_ = nullptr;

	// list of diagnostics for Particles
	std::forward_list<diagnostic_up> diagnostics_;
};


//#################################################################################################

#endif // MANAGERS_PARTICLES_HPP
