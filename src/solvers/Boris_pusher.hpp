#ifndef BORIS_PUSHER_HPP
#define BORIS_PUSHER_HPP

#include "abstract_strategies.hpp"

#include "src/pch.h"
#include "src/particles/particle/particle.hpp"
#include "src/vectors/vector_classes.hpp"

class Boris_pusher : public Pusher {
public:
  Boris_pusher() = default;
  void process(Particle&, const vector3& local_E, const vector3& local_B) const override;
};

#endif // BORIS_PUSHER_HPP
