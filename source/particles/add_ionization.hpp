#ifndef ADD_IONIZATION_HPP
#define ADD_IONIZATION_HPP

//#################################################################################################

#include <vector>
#include <functional>

#include "particles.hpp"


class Ionization {
public:
    Ionization (
        std::vector<int> array_of_particles_to_load,
        std::function<void(double* x, double* y)>&& set_point_of_birth,
        std::function<double(double x, double y)>&& get_probability,
        std::function<void(double x, double y, double mass, double Tx, double Ty, double Tz,
          double p0, double* px, double* py, double* pz)>&& load_impulse )
            :   array_of_particles_to_load_(std::move(array_of_particles_to_load)),
                set_point_of_birth_(std::move(set_point_of_birth)),
                get_probability(std::move(get_probability)),
                load_impulse_(std::move(load_impulse)) {};

    void process (Particles* const ionized, Particles* const lost, int t);

private:
    const std::vector<int> array_of_particles_to_load_;
    std::function<void(double* x, double* y)> set_point_of_birth_;
    std::function<double(double x, double y)> get_probability;
    std::function<void(double x, double y, double mass, double Tx, double Ty, double Tz,
        double p0, double* px, double* py, double* pz)> load_impulse_;
};


double uniform_probability(double x, double y);
void set_point_on_circle(double* x, double* y);
std::vector<int> set_time_distribution(int t_inj, std::size_t total_number_of_particles);

//#################################################################################################

#endif // ADD_IONIZATION_HPP