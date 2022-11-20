#ifndef GLOBAL_PARAMETERS_HPP
#define GLOBAL_PARAMETERS_HPP

//#################################################################################################

#include "src/pch.h"

double second_order_spline(double x, double grid_mesh);
double third_order_spline(double x, double grid_mesh);
double fourth_order_spline(double x, double grid_mesh);
double fifth_order_spline(double x, double grid_mesh);


class Variadic_parameter {
public:
	Variadic_parameter() = delete;

	Variadic_parameter(const std::string& type) : parameter_type(type) {};

	virtual ~Variadic_parameter() = default;
	
	virtual double
	get_value(size_t id) const = 0;
	
	virtual void
	set_value(double value) = 0;

	const std::string&
	get_type() { return parameter_type; }

private:
	std::string parameter_type;
};


class Global_parameter : public Variadic_parameter {
public:
	Global_parameter(double value) : Variadic_parameter("global"), value_(value) {};

	double
	get_value(size_t id) const override { return value_; }

	void
	set_value(double value) override { value_ = value; }

private:
	double value_;
};


class Local_parameter : public Variadic_parameter {
public:
	Local_parameter() : Variadic_parameter("local") {};

	double
	get_value(size_t id) const override { return values_[id]; }

	void
	set_value(double value) override { values_.emplace_back(value); }

private:	
	std::vector<double> values_;
};


class Parameters {
public:
	Parameters() = default;
	~Parameters() = default;

	/**
	 * Constructor.
	 * 
	 * @param m  Mass of the particles in a sort.
	 * @param Np Number of particles in cell.
	 * @param Tx Temperature in x direction.
	 * @param Ty Temperature in y direction.
	 * @param Tz Temperature in z direction.
	 * @param p0 Initial impulse (absolute value).
	 * @param charge_cloud Width of the spline.
	 * @param form_factor Shape of the macro-particle.
	 */
	Parameters(
		int Np, double m, std::unique_ptr<Variadic_parameter> n, std::unique_ptr<Variadic_parameter> q,
		double p0, double Tx, double Ty, double Tz, int charge_cloud = 3,
		std::function<double(double, double)> form_factor = second_order_spline)
		: Np_(Np), m_(m), n_(std::move(n)), q_(std::move(q)), p0_(p0),
		  charge_cloud_(charge_cloud), form_factor_(form_factor)
		  {
			  T_[x] = Tx;
			  T_[y] = Ty;
			  T_[z] = Tz;
		  };

	Parameters(Parameters&& other) = default;
	Parameters& operator=(Parameters&& other) = default;

	int Np() 	const { return Np_;   }
	double m()  const { return m_; 	  }
	double p0() const { return p0_;   }
	double Tx() const { return T_[x]; }
	double Ty() const { return T_[y]; }
	double Tz() const { return T_[z]; }
	int charge_cloud() const { return charge_cloud_; }
	const auto& form_factor() const { return form_factor_; }
	
	
	double n(size_t id) const { return n_->get_value(id); }
	double q(size_t id) const { return q_->get_value(id); }

	void set_n(double value) { n_->set_value(value); }
	void set_q(double value) { q_->set_value(value); }

	const std::string& n_type() { return n_->get_type(); }
	const std::string& q_type() { return q_->get_type(); }

	std::string sort_name_;

private:
	int Np_;
	double m_;
	
	std::unique_ptr<Variadic_parameter> n_;
	std::unique_ptr<Variadic_parameter> q_;

	double p0_;
	double T_[3];
	
	int charge_cloud_;
	std::function<double(double, double)> form_factor_;

	enum Axis { x = 0, y, z };
};


//#################################################################################################

#endif //GLOBAL_PARAMETERS_HPP
