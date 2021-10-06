#ifndef ESIRKEPOV_DENSITY_DECOMPOSITION_HPP
#define ESIRKEPOV_DENSITY_DECOMPOSITION_HPP

//#################################################################################################

#include "abstract_strategies.hpp"

#include <omp.h>
#include <cmath>
#include <functional>

#include "../particles/particle/particle_parameters.hpp"
#include "../particles/particle/point.hpp"
#include "../vectors/vector3_field.hpp"
#include "../constants.h"

using p_v3f = px_py_vector3_field;


class Esirkepov_density_decomposition : public Decomposition {
public:
	Esirkepov_density_decomposition(const Particle_parameters& parameters, vector3_field& J)
		:	q_(parameters.q()), m_(parameters.m()),	n_(parameters.n()),	Np_(parameters.Np()),
			charge_cloud_(parameters.charge_cloud()),
			shape_at_(parameters.form_factor()),
            J_(J) {};

	void process(const Point&, const vector2& r0) override;

private:
	void decompose_x(const vector2& r, const vector2& r0, int nx, int ny, p_v3f& temp_J);
	void decompose_y(const vector2& r, const vector2& r0, int nx, int ny, p_v3f& temp_J);
	void decompose_z(const Point&, const vector2& r0, int nx, int ny, p_v3f& temp_J);
	
	const double q_;
    const double m_;
	const double n_;
	const int 	Np_;
	const int charge_cloud_;
	const std::function<double(double, double)> shape_at_;
    vector3_field& J_;
};

//#################################################################################################

#endif // ESIRKEPOV_DENSITY_DECOMPOSITION_HPP