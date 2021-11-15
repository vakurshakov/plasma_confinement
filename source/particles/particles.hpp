#ifndef MANAGERS_PARTICLES_HPP
#define MANAGERS_PARTICLES_HPP

//#################################################################################################

#include <memory>
#include <vector>
#include <string>
#include <functional>

#include "./particle/particle_parameters.hpp"
#include "./particle/point.hpp"
#include "../diagnostics/diagnostics.hpp"
#include "../solvers/abstract_strategies.hpp" 
#include "../vectors/vector3_field.hpp"
#include "../constants.h"


class Particles {
public:
	using diagnostic_up = std::unique_ptr<Particles_diagnostic>;

	Particles() = default;
	Particles(
		Particle_parameters&,
		std::unique_ptr<Pusher>,
		std::unique_ptr<Interpolation>,
		std::unique_ptr<Decomposition>,
		std::function<void(Point&, double)>&& x_boundary,
		std::function<void(Point&, double)>&& y_boundary,
		std::vector<diagnostic_up>&&	);
	const auto& get_parameters() const { return parameters_; }
	const auto& get_points()	 const { return points_; } 

	// main Particles methods
	void push();
	void diagnose(int t) const;
	
	friend class Set_particles;
	friend class Copy_coordinates;
	friend class Ionize_particles;

private:
	// main Kinetic_particles fields
	Particle_parameters parameters_;
	std::vector<Point> points_;

	// solvers for particles
	std::unique_ptr<Pusher> push_;
	std::unique_ptr<Interpolation> interpolation_;
	std::unique_ptr<Decomposition> decomposition_;
	
	void boundaries_processing(Point&, double size_x, double size_y);
	std::function<void(Point&, double)> x_boundary_ = nullptr;
	std::function<void(Point&, double)> y_boundary_ = nullptr;	

	// vector of diagnostics for Particles
	std::vector<diagnostic_up> diagnostics_;
};


//#################################################################################################

#endif // MANAGERS_PARTICLES_HPP
