#include "particles.hpp"

#include <memory>
#include <vector>
#include <string>
#include <cstdarg>
#include <functional>

#include "./particle/particle.hpp"
#include "../diagnostics/diagnostics.hpp"
#include "../solvers/abstract_strategies.hpp"
#include "../solvers/Boris_pusher.hpp"
#include "../vectors/vector_classes.hpp"
#include "../constants.h"


Particles::Particles(
	Parameters&& parameters,
	std::unique_ptr<Pusher> push,
	std::unique_ptr<Interpolation> interpolation,
	std::unique_ptr<Decomposition> decomposition,
	std::function<void(Point&, double)>&& x_boundary,
	std::function<void(Point&, double)>&& y_boundary,
	std::vector<diagnostic_up>&& diagnostics ) 
	: 	parameters_(parameters),
	  	push_(std::move(push)),
	  	interpolation_(std::move(interpolation)),
	  	decomposition_(std::move(decomposition)),
		x_boundary_(std::move(x_boundary)),
		y_boundary_(std::move(y_boundary)),
		diagnostics_(std::move(diagnostics)) {};


void Particles::push()
{
    auto push		  = std::mem_fn(&Pusher::process);
    auto interpolate  = std::mem_fn(&Interpolation::process);
	auto decompose	  = std::mem_fn(&Decomposition::process);
	
	#pragma omp parallel for shared(particles_), num_threads(THREAD_NUM),	\
		firstprivate(push, interpolate, decompose, x_boundary_, y_boundary_)
	for (auto& particle : particles_) {
		
		const vector2 r0 = particle.get_point().r();
		
		{
			vector3 local_E = { 0., 0., 0. };
			vector3 local_B = { 0., 0., 0. };

			interpolate(interpolation_, r0, local_E, local_B);
			push(push_, particle, local_E, local_B);
		}

		decompose(decomposition_, particle, r0);		
		boundaries_processing(particle.get_point(), SIZE_X*dx, SIZE_Y*dy);
	}
}

void Particles::boundaries_processing(Point& point, double size_x, double size_y)
{
	x_boundary_(point, size_x);
	y_boundary_(point, size_y);
}


void Particles::diagnose(int t) const
{
	#pragma omp parallel for shared(diagnostics_), num_threads(THREAD_NUM)
	for (auto& diagnostic : diagnostics_) {
		diagnostic->diagnose(parameters_, particles_, t);
	} 
}

/**
 * @brief Adds a particle to the end of the array.
 * 
 * @param point Coordinates and impulse of the particle.
 * @param ... Local parameters.
 */
void Particles::add_particle(const Point& point, ...)
{
	va_list list;
	va_start(list, point);
	
	if (parameters_.n_type() == "local")
		parameters_.set_n(static_cast<double>(va_arg(list, double)));

	if (parameters_.q_type() == "local")
		parameters_.set_q(static_cast<double>(va_arg(list, double)));

	va_end(list);

	particles_.emplace_back(Particle{number_in_sort++, point, parameters_});
}