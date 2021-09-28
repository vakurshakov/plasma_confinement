#include "particles.hpp"

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <forward_list>

#include "./particle/particle_parameters.hpp"
#include "./particle/point.hpp"
#include "../command/command.hpp"
#include "../diagnostics/diagnostics.hpp"
#include "../vectors/vector_classes.hpp"
#include "../constants.h"


using diagnostic_up = std::unique_ptr<Diagnostic>;
using pcommand_up = std::unique_ptr<Particle_command>;
using Pusher_up = std::unique_ptr<Boris_pusher>;


Particles::Particles(
	Particle_parameters& parameters,
	std::vector<Point>&& points,
	Pusher_up& pusher,
	std::function<void(Point&, double)>&& x_boundary,
	std::function<void(Point&, double)>&& y_boundary,
	std::forward_list<diagnostic_up>&& diagnostics )
		: parameters_(std::move(parameters)),
		  points_(std::move(points)),
		  pusher_(std::move(pusher)),
		  x_boundary_(std::move(x_boundary)),
		  y_boundary_(std::move(y_boundary)),
		  diagnostics_(std::move(diagnostics)) {};


void Particles::set_push_commands(std::forward_list<pcommand_up>&& commands) {
	push_commands_.swap(commands);
}


void Particles::push()
{
	if (!push_commands_.empty()) {

		#pragma omp parallel for num_threads(THREAD_NUM)
		for (auto point : points_) {

			const vector2 r0 = point.r();
			for (auto& command : push_commands_) {
				command->execute(point, r0);
			}
			this->boundaries_processing(point, SIZE_X*dx, SIZE_Y*dy);
		}
	}
}


void Particles::boundaries_processing(Point& point, double size_x, double size_y)
{
	x_boundary_(point, size_x);
	y_boundary_(point, size_y);
}


void Particles::diagnose(int t)
{
	if (!diagnostics_.empty()) {
		for (auto& diagnostic : diagnostics_) {
			diagnostic->diagnose(parameters_, points_, t);
		} 
	}
}
