#ifndef ABSTRACT_STRATEGIES_HPP
#define ABSTRACT_STRATEGIES_HPP

//#################################################################################################

#include "../vectors/vector_classes.hpp"
#include "../particles/particle/point.hpp"


class Interpolation {
public:
    Interpolation() = default;
    virtual ~Interpolation() = default;
    virtual void process(const vector2& r0, vector3& local_E, vector3& local_B) const {};
};


class Pusher {
public:
    Pusher() = default;
    virtual ~Pusher() = default;
    virtual void process(Point&, const vector3& local_E, const vector3& local_B) const {};
};


class Decomposition {
public:
    Decomposition() = default;
    virtual ~Decomposition() = default;
    virtual void process(const Point&, const vector2& r0) {};
};

//#################################################################################################

#endif // ABSTRACT_STRATEGIES_HPP