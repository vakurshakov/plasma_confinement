#ifndef ESIRKEPOV_DENSITY_DECOMPOSITION_HPP
#define ESIRKEPOV_DENSITY_DECOMPOSITION_HPP

//#################################################################################################

#include "abstract_strategies.hpp"

#include <omp.h>
#include <cmath>
#include <functional>

#include "../particles/particle/particle.hpp"
#include "../vectors/vector3_field.hpp"
#include "../constants.h"


class Esirkepov_density_decomposition : public Decomposition {
public:
	Esirkepov_density_decomposition(const Parameters& parameters, vector3_field& J)
		:	Np_(parameters.Np()),
			charge_cloud_(parameters.charge_cloud()),
			shape_at_(parameters.form_factor()),
            J_(J) {};

	void process(const Particle&, const vector2& r0) override;

private:
	using p_v3f = px_py_vector3_field;
	void decompose_x(const Particle&, const vector2& r0, double n, double q, int nx, int ny, p_v3f& temp_J);
	void decompose_y(const Particle&, const vector2& r0, double n, double q, int nx, int ny, p_v3f& temp_J);
	void decompose_z(const Particle&, const vector2& r0, double n, double q, int nx, int ny, p_v3f& temp_J);

	const int Np_;
	const int charge_cloud_;
	const std::function<double(double, double)> shape_at_;
    vector3_field& J_;
};

//#################################################################################################

#endif // ESIRKEPOV_DENSITY_DECOMPOSITION_HPP