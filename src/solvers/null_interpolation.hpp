#ifndef NULL_INTERPOLATION_HPP
#define NULL_INTERPOLATION_HPP

#include "abstract_strategies.hpp"

#include "src/pch.h"
#include "src/vectors/vector_classes.hpp"

class Null_interpolation : public Interpolation {
 public:
  Null_interpolation() = default;
  void process(const vector2& r0, vector3& local_E, vector3& local_B) const override {}
};

#endif // NULL_INTERPOLATION_HPP
