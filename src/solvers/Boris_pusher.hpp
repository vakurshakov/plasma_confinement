#ifndef BORIS_PUSHER_HPP
#define BORIS_PUSHER_HPP

//#################################################################################################

#include "abstract_strategies.hpp"

#include "../particles/particle/particle.hpp"
#include "../vectors/vector3_field.hpp"
#include "../vectors/vector_classes.hpp"
#include "../constants.h"


class Boris_interpolation : public Interpolation {
public:
	using v3f = vector3_field;
	
	Boris_interpolation(const Parameters& parameters, const v3f& E, const v3f& B)
		:   E_(E), B_(B),
            shape_at_(parameters.form_factor()),
            charge_cloud_(parameters.charge_cloud()) {};

	void process(const vector2& r0, vector3& local_E, vector3& local_B) const override;

private:
	const v3f& E_;
	const v3f& B_;
    const std::function<double(double, double)> shape_at_;
	const int charge_cloud_;
};

class Boris_pusher : public Pusher {
public:
	Boris_pusher() = default;
	void process(Particle&, const vector3& local_E, const vector3& local_B) const override;
};

//#################################################################################################

#endif // BORIS_PUSHER_HPP