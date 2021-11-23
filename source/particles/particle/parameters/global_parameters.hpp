#ifndef GLOBAL_PARAMETERS_HPP
#define GLOBAL_PARAMETERS_HPP

//#################################################################################################

#include <stdexcept>
#include <functional>


double second_order_spline(double x, double grid_mesh);
double third_order_spline(double x, double grid_mesh);
double fourth_order_spline(double x, double grid_mesh);
double fifth_order_spline(double x, double grid_mesh);


/**
 * Major variables for each particle.
 * 
 * @brief This is the class that contains a major 
 * 		constant variables for every particle in a sort. 
 */
class gParameters {
private: 
	// Enumerator to access to a double[3] array of temperature.
	enum Axis { x, y, z };
	
public:
	// Default constructor.
	gParameters() = default;

	// Destructor.
	virtual ~gParameters() = default;

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
	gParameters(
		double m, int Np, double Tx, double Ty, double Tz, double p0,
		int charge_cloud = 2,
		std::function<double(double, double)> form_factor = second_order_spline)
		: m_(m), Np_(Np), p0_(p0),
		  charge_cloud_(charge_cloud),
		  form_factor_(form_factor)
		  {
			T_[x] = Tx;
			T_[y] = Ty;
			T_[z] = Tz;
		  };

	// Getters for every field in a class.

	double m() 	const { return m_; }
	int    Np()	const { return Np_; }
	double Tx() const { return T_[x]; }
	double Ty() const { return T_[y]; }
	double Tz() const { return T_[z]; }
	double p0() const { return p0_; }
	int charge_cloud() const { return charge_cloud_; }
	const auto& form_factor() const { return form_factor_; }

	//  For the methods below, you should choose whether the particle will have local
	//		parameters (for each one individually, as suggested below) or global ones.
	
	// Density getter.
	virtual double n() const { throw std::runtime_error("Access to unset global density."); }
	
	// Charge getter.
	virtual double q() const { throw std::runtime_error("Access to unset global charge.");  }

protected:
	double m_;
	int    Np_;
	double T_[3];
	double p0_;
	int charge_cloud_;
	std::function<double(double, double)> form_factor_;
};

//#################################################################################################

#endif //GLOBAL_PARAMETERS_HPP
