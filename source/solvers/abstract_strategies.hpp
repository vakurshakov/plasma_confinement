#ifndef ABSTRACT_STRATEGIES_HPP
#define ABSTRACT_STRATEGIES_HPP

//#################################################################################################

#include "../vectors/vector_classes.hpp"
#include "../particles/particle/concrete/particle_interface.hpp"


class Interpolation {
public:
    virtual ~Interpolation() = default;
    virtual void process(const vector2& r0, vector3& local_E, vector3& local_B) const = 0;
};


class Pusher {
public:
    virtual ~Pusher() = default;
    virtual void process(IParticle&, const vector3& local_E, const vector3& local_B) const = 0;
};


class Decomposition {
public:
    virtual ~Decomposition() = default;
    virtual void process(const IParticle&, const vector2& r0) = 0;
};

//#################################################################################################

#endif // ABSTRACT_STRATEGIES_HPP