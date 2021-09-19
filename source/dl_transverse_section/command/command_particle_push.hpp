#include "./command.hpp"

#include <functional>

#include "../particle/particle_parameters.hpp"
#include "../particle/point.hpp"
#include "../vectors/vector3_field.hpp"

using v3f = vector3_field;


class Particle_push : public Command {
public:
	Particle_push(
		std::function<void(const Particle_parameters&, Point&, const v3f& E, const v3f& B)>,
		const Particle_parameters&, const Point&, const v3f& E, const v3f& B 	);

	void execute() override {
		this->particle_pusher(parameters_, point_, E_, B_);
	};

private:
	auto particle_pusher;
	Particle_parameters* parameters_;
	Point* point_; 
	v3f *E_, *B_;	
};