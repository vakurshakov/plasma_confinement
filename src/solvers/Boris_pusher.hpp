#ifndef SOLVERS_BORIS_PUSHER_HPP
#define SOLVERS_BORIS_PUSHER_HPP

//#######################################################################################

#include "./solvers.hpp"

#include <cmath>
#include <functional>

#include "../particle/particle_parameters.hpp"
#include "../particle/point.hpp"
#include "../vectors/vector3_field.hpp"


using v3f = vector3_field;


class Boris_pusher {
public:
	Boris_pusher() = default;

	void interpolate(const v3f& E, const v3f& B, const vector2& r0);
	void push(const Particle_parameters&, Point*);

private:
	vector3 local_E, local_B;
	
	vector2 r_;
	vector3 p_;		
};

//#######################################################################################

#endif // SOLVERS_BORIS_PUSHER_HPP