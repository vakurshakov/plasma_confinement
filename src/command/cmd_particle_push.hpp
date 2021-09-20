#ifndef CMD_PARTICLE_PUSH_HPP
#define CMD_PARTICLE_PUSH_HPP

//#######################################################################################

#include "./command.hpp"

#include <functional>

#include "../particle/particle_parameters.hpp"
#include "../particle/point.hpp"
#include "../solvers/solvers.hpp"
#include "../vectors/vector3_field.hpp"
#include "../vectors/vector_classes.hpp"

using v3f = vector3_field;


class Interpolate_fields : public Particle_command {
public:
	Interpolate_fields(Boris_pusher* pusher, const v3f& E, const v3f& B,
		const Particle_parameters& parameters)
			: pusher_(pusher), E_(E), B_(B), parameters_(parameters) {};

	void execute(Point* point, const vector2& r0) const override {
		this->pusher_->interpolate(E_, B_, parameters_, r0);
	};

private:
	Boris_pusher* pusher_;
	const v3f& E_;
	const v3f& B_;
	const Particle_parameters& parameters_;
};

class Push_particle : public Particle_command {
public:
	Push_particle(Boris_pusher* pusher, const Particle_parameters& parameters)
		: pusher_(pusher), parameters_(parameters) {};

	void execute(Point* point, const vector2&) const override {
		this->pusher_->push(parameters_, point);
	};

private:
	Boris_pusher* pusher_;
	const Particle_parameters& parameters_;
};

//#######################################################################################

#endif // CMD_PARTICLE_PUSH_HPP
