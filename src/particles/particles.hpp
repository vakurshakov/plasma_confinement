#ifndef SRC_PARTICLES_PARTICLES_HPP
#define SRC_PARTICLES_PARTICLES_HPP

#include "src/pch.h"
#include "src/particles/particle/particle.hpp"
#include "src/solvers/abstract_strategies.hpp"
#include "src/diagnostics/diagnostics.hpp"
#include "src/particles/particles_builder.hpp"

#include "src/particles/particle-boundary_processor.hpp"

class Particles {
public:
	using diagnostic_up = std::unique_ptr<Particles_diagnostic>;

	Particles() = default;
	
	Particles(Particles_builder& builder);
	
	const std::vector<Particle>& get_particles() const { return particles_; }
	const Parameters& get_parameters() const { return parameters_; }

	void add_particle(const Point& point, ...);
	void add_diagnostic(diagnostic_up&& diagnostic);

	void push();
	void diagnose(int t) const;
	
	friend class Set_particles;
	friend class Copy_coordinates;
	friend class Ionize_particles;

	/// @todo refactor this out
	friend class Clone_layer_particles;
	friend class Plasma_boundary_processor;
	friend class Beam_boundary_processor;

private:
	Parameters parameters_;
	std::vector<Particle> particles_;

	std::unique_ptr<Pusher> push_;
	std::unique_ptr<Interpolation> interpolation_;
	std::unique_ptr<Decomposition> decomposition_;

	std::unique_ptr<Particle_boundary_processor> boundaries_processor_;

	std::vector<diagnostic_up> diagnostics_;
};

#endif  // SRC_PARTICLES_PARTICLES_HPP
