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
		double q, double m, double n, int Np,
		double Tx, double Ty, double Tz, double p0,
		int charge_cloud = 3,
		std::function<double(double, double)> form_factor = second_order_spline)
		: q_ (q), m_ (m), n_ (n), Np_(Np),
		  Tx_(Tx), Ty_(Ty), Tz_(Tz), p0_(p0),
		  charge_cloud_(charge_cloud),
		  form_factor_(form_factor) {};

	// getters
	double  q()	const { return q_; }
	double  m()	const { return m_; }
	double  n()	const { return n_; }
	int    Np()	const { return Np_; }
	double Tx() const { return Tx_; }
	double Ty() const { return Ty_; }
	double Tz() const { return Tz_; }
	double p0() const { return p0_; }
	int charge_cloud() const { return charge_cloud_; }
	auto form_factor() const { return form_factor_; }

	double& n() { return n_; }

private:
	// sort parameters
	double q_;
	double m_;
	double n_;
	int Np_;
	double Tx_, Ty_, Tz_;
	double p0_;
	int charge_cloud_;
	std::function<double(double, double)> form_factor_;
};

//#################################################################################################

#endif
