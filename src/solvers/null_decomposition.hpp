#ifndef NULL_DECOMPOSITION_HPP
#define NULL_DECOMPOSITION_HPP

#include "abstract_strategies.hpp"

#include "src/pch.h"
#include "src/particles/particle/particle.hpp"
#include "src/vectors/vector_classes.hpp"

class Null_decomposition : public Decomposition {
 public:
  Null_decomposition() = default;
  void process(const Particle&, const vector2& r0) {}
};

#endif // NULL_DECOMPOSITION_HPP
