#ifndef GLOBAL_PARAMETERS_HPP
#define GLOBAL_PARAMETERS_HPP

//#################################################################################################

#include <functional>
#include <memory>


double second_order_spline(double x, double grid_mesh);
double third_order_spline(double x, double grid_mesh);
double fourth_order_spline(double x, double grid_mesh);
double fifth_order_spline(double x, double grid_mesh);


struct Parameter {
	virtual ~Parameter() = default;
	
protected:
	Parameter() = default;
};


struct Global_parameter : public Parameter {
	Global_parameter(double value) : value_(value) {};
	double value_;
};


struct Local_parameter : public Parameter {
	Local_parameter() = default;
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
	 * @param form_factor Shape of the macroparticle.
	 */
	Parameters(
		int Np, double m, std::unique_ptr<Parameter> n, std::unique_ptr<Parameter> q,
		double p0, double Tx, double Ty, double Tz, int charge_cloud = 2,
		std::function<double(double, double)> form_factor = second_order_spline)
		: Np_(Np), m_(m), n_(std::move(n)), q_(std::move(q)), p0_(p0),
		  charge_cloud_(charge_cloud), form_factor_(form_factor)
		  {
			  T_[x] = Tx;
			  T_[y] = Ty;
			  T_[z] = Tz;
		  };

	Parameters(Parameters& other)
	{
		Np_   = other.Np_;
		m_    = other.m_;
		p0_   = other.p0_;
		T_[x] = other.T_[x];
		T_[y] = other.T_[y];
		T_[z] = other.T_[z];
		charge_cloud_ = other.charge_cloud_;
		form_factor_  = other.form_factor_;

		n_.swap(other.n_);
		q_.swap(other.q_);
	}

	int Np() 	const { return Np_;   }
	double m()  const { return m_; 	  }
	double p0() const { return p0_;   }
	double Tx() const { return T_[x]; }
	double Ty() const { return T_[y]; }
	double Tz() const { return T_[z]; }
	int charge_cloud() const { return charge_cloud_; }
	const auto& form_factor() const { return form_factor_; }
	
	
	Parameter* raw_n() const { return n_.get(); }
	double n(size_t id) const
	{
		if (auto* n = dynamic_cast<Global_parameter*>(raw_n()); n != nullptr)
			return n->value_;

		else if (auto* n = dynamic_cast<Local_parameter*>(raw_n()); n != nullptr)
			return n->values_[id];
	}

	Parameter* raw_q() const { return q_.get(); }
	double q(size_t id) const
	{
		if (auto* q = dynamic_cast<Global_parameter*>(raw_q()); q != nullptr)
			return q->value_;

		else if (auto* q = dynamic_cast<Local_parameter*>(raw_q()); q != nullptr)
			return q->values_[id];
	}


private:
	int Np_;
	double m_;
	
	std::unique_ptr<Parameter> n_;
	std::unique_ptr<Parameter> q_;

	double p0_;
	double T_[3];
	
	int charge_cloud_;
	std::function<double(double, double)> form_factor_;

	enum Axis { x = 0, y, z };
};


//#################################################################################################

#endif //GLOBAL_PARAMETERS_HPP
