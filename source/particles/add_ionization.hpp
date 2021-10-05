#ifndef ADD_IONIZATION_HPP
#define ADD_IONIZATION_HPP

//#################################################################################################

#include <vector>
#include <functional>

#include "particles.hpp"


class Ionization {
public:
    Ionization (
        int total_amount_of_particles_to_load, int time_of_injection,
        std::function<void(double* x, double* y)>&& set_point_of_birth,
        std::function<double(double x, double y)>&& get_probability,
        std::function<void(double x, double y, double mass, double Tx, double Ty, double Tz,
          double p0, double* px, double* py, double* pz)>&& load_impulse)
            :   number_of_particles_to_load_(total_amount_of_particles_to_load/time_of_injection),
                set_point_of_birth_(set_point_of_birth),
                get_probability(get_probability),
                load_impulse_(load_impulse) {};

    void process (Particles* const ionized, Particles* const lost);

private:
    int number_of_particles_to_load_;
    std::function<void(double* x, double* y)> set_point_of_birth_;
    std::function<double(double x, double y)> get_probability;
    std::function<void(double x, double y, double mass, double Tx, double Ty, double Tz,
        double p0, double* px, double* py, double* pz)> load_impulse_;
};


double uniform_probability(double x, double y);
void set_point_on_circle(double* x, double* y);


//#################################################################################################

#endif // ADD_IONIZATION_HPP