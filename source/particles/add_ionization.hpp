#ifndef ADD_IONIZATION_HPP
#define ADD_IONIZATION_HPP

//#################################################################################################

#include <vector>
#include <functional>

#include "particles.hpp"


class Ionization {
public:
    Ionization (
	    std::function<bool(int nx, int ny)>&& this_is_config_cell,
      std::function<void(int sequential_number, int Np,
          int cell_number_nx, int cell_number_ny, double* x, double* y)>&& fill_the_cell,
      std::function<void(double x, double y, double mass, double Tx, double Ty, double Tz,
        double p0, double* px, double* py, double* pz)>&& load_impulse,
      std::function<double(int nx, int ny)>&& density_shape )
      // const std::function<double(double nx, double ny)>& probability
          : this_is_config_cell_(this_is_config_cell),
            fill_the_cell_(fill_the_cell),
            load_impulse_(load_impulse),
            density_shape_(density_shape) {};

    void process (Particles* const ionized, Particles* const lost);
    double check_density(Particles* const ionized, int nx, int ny);

private:
    std::function<bool(int nx, int ny)> this_is_config_cell_;
    std::function<void(int sequential_number, int Np,
	    int cell_number_nx, int cell_number_ny, double* x, double* y)> fill_the_cell_;
    std::function<void(double x, double y, double mass, double Tx, double Ty, double Tz,
    	double p0, double* px, double* py, double* pz)> load_impulse_;
    std::function<double(int nx, int ny)> density_shape_;
};

double uniform_density(int nx, int ny);


//#################################################################################################

#endif // ADD_IONIZATION_HPP