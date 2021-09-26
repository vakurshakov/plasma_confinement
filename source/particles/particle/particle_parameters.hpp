#ifndef PARTICLE_PARAMETERS_H
#define PARTICLE_PARAMETERS_H

//#################################################################################################

#include <functional>

double frand();

double second_order_spline(double x, double grid_mesh);
double third_order_spline(double x, double grid_mesh);
double fourth_order_spline(double x, double grid_mesh);
double fifth_order_spline(double x, double grid_mesh);


class Particle_parameters {
public:
	Particle_parameters() = default;

	Particle_parameters(
		double q, double m, double n, int Np, int charge_cloud = 3,
		std::function<double(double, double)> form_factor = second_order_spline)
		: 	q_(q), m_(m), n_(n), Np_(Np),
			charge_cloud_(charge_cloud), form_factor_(form_factor) {};

	// getters
	double q()	const { return q_; };
	double m()	const { return m_; };
	double n()	const { return n_; };
	int   Np()	const { return Np_; };
	int charge_cloud() const { return charge_cloud_; };
	auto form_factor() const { return form_factor_; };

	double& n() { return n_; };

protected:
	// sort parameters
	double q_;
	double m_;
	double n_;
	double Np_;
	int charge_cloud_;
	std::function<double(double, double)> form_factor_;
};

//#################################################################################################

#endif
