#include "./particles.hpp"

#include <memory>
#include <vector>
#include <functional>
#include <forward_list>

#include "../command/command.hpp"
#include "../diagnostics/diagnostics.hpp"
#include "../particle/particle_parameters.hpp"
#include "../particle/point.hpp"
#include "../vectors/vector3_field.hpp"
#include "../vectors/vector_classes.hpp"
#include "../constants.h"


using v3f = vector3_field;
using diagnostic_up = std::unique_ptr<Diagnostic>;
using pcommand_up = std::unique_ptr<Particle_command>;


Particles::Particles(
	Particle_parameters parameters,
	std::vector<Point>&& points,
	Boris_pusher* pusher,
	std::function<void(Point*, double)>&& x_boundary,
	std::function<void(Point*, double)>&& y_boundary,
	std::forward_list<diagnostic_up>&& diagnostics )
{
	parameters_ = parameters;
	points_.swap(points);
	pusher_ = pusher;
	x_boundary_ = x_boundary;
	y_boundary_ = y_boundary;
	diagnostics_.swap(diagnostics);
}


void Particles::set_push_commands(std::forward_list<pcommand_up>&& commands) {
	push_commands_.swap(commands);
}


void Particles::push()
{
	if (!push_commands_.empty()) {
		
		#pragma omp parallel for num_threads(THREAD_NUM)
		for (decltype(points_.size()) i = 0; i < points_.size(); ++i) {
			
			const vector2 r0 = points_[i].r();
			for (auto& command : push_commands_) {
				command->execute(&points_[i], r0);
			}
		
			this->boundaries_processing(i, SIZE_X*dx, SIZE_Y*dy);
		}
	}
}


void Particles::boundaries_processing(int i, double size_x, double size_y)
{
	x_boundary_(&points_[i], size_x);
	y_boundary_(&points_[i], size_y);
}


void Particles::diagnose(int t)
{
	if (!diagnostics_.empty()) {
		for (auto& diagnostic : diagnostics_) {
			diagnostic->diagnose(parameters_, points_, t);
		} 
	}
}
