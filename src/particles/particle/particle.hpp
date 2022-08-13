#ifndef PARTICLE_INTERFACE_HPP 
#define PARTICLE_INTERFACE_HPP

//#################################################################################################

#include "point.hpp"
#include "parameters.hpp"


class Particle {
public:
    Particle() = delete;

    Particle(size_t id, const Point& point, const Parameters& parameters)
    :   id(id), point(point), parameters(parameters) {}

    Point& get_point() { return point; } 
    const Point& get_point() const { return point; }

    double n() const { return parameters.n(id); };
    double q() const { return parameters.q(id); };
    double m() const { return parameters.m(); }

protected:
    size_t id;
    Point point;
    const Parameters& parameters;
};


//#################################################################################################

#endif //PARTICLE_INTERFACE_HPP