#include "./command.hpp"

#include <functional>

#include "../particle/particle_parameters.hpp"
#include "../particle/point.hpp"
#include "../vectors/vector3_field.hpp"

using v3f = vector3_field;


class Particle_push : public Command {
public:
	Particle_push(
		std::function<void(const Particle_parameters&, const Point&, const vector2& r0, v3f& J)>,
		const Particle_parameters&, const Point&, const vector2& r0, v3f& J 	);

	void execute() override {
		this->density_decomposer(parameters_, point_, r0_, J_);
	};

private:
	auto density_decomposer;
	Particle_parameters* parameters_;
	Point* point_; 
	vector2 r0_;
	v3f J_;	
};