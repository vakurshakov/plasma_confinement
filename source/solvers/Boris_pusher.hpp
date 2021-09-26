#ifndef SOLVERS_BORIS_PUSHER_HPP
#define SOLVERS_BORIS_PUSHER_HPP

//#######################################################################################

#include "solvers.hpp"

#include <cmath>
#include <functional>

#include "../particles/particle/particle_parameters.hpp"
#include "../particles/particle/point.hpp"
#include "../vectors/vector_classes.hpp"
#include "../vectors/vector3_field.hpp"


using v3f = vector3_field;


class Boris_pusher {
public:
	Boris_pusher() = default;

	void interpolate(const v3f* const E, const v3f* const B,
		const Particle_parameters&, const vector2& r0);
	
	void push(const Particle_parameters&, Point&);

private:
	vector3 local_E = {0, 0, 0};
	vector3 local_B = {0, 0, 0};
};

//#######################################################################################

#endif // SOLVERS_BORIS_PUSHER_HPP
