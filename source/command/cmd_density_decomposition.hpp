#ifndef CMD_DENSITY_DECOMPOSITION_HPP
#define CMD_DENSITY_DECOMPOSITION_HPP

//#######################################################################################

#include "command.hpp"

#include <functional>

#include "../particles/particle/particle_parameters.hpp"
#include "../particles/particle/point.hpp"
#include "../solvers/solvers.hpp"
#include "../vectors/vector3_field.hpp"
#include "../vectors/vector_classes.hpp"

using v3f = vector3_field;


class Density_decomposition : public Particle_command {
public:
	Density_decomposition(
		std::function<void(const Particle_parameters&,
			const Point&, const vector2& r0, v3f& J)>&& decompose,
		const Particle_parameters& parameters, v3f& J )
			: decompose_(std::move(decompose)), parameters_(parameters), J_(J) {};

	void execute(Point& point, const vector2& r0) const override {
		this->decompose_(parameters_, point, r0, J_);
	};

private:
	const std::function<void(const Particle_parameters&,
		const Point&, const vector2& r0, v3f& J)> decompose_;
	const Particle_parameters& parameters_;
	v3f& J_;	
};


//#######################################################################################

#endif // CMD_DENSITY_DECOMPOSITION_HPP