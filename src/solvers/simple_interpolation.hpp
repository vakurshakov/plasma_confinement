#ifndef SIMPLE_INTERPOLATION_HPP
#define SIMPLE_INTERPOLATION_HPP

#include "abstract_strategies.hpp"

#include "src/pch.h"
#include "src/particles/particle/particle.hpp"
#include "src/vectors/vector3_field.hpp"
#include "src/vectors/vector_classes.hpp"

class Simple_interpolation : public Interpolation {
  using v3f = vector3_field;

 public:
  Simple_interpolation(const v3f& E, const v3f& B);
  void process(const vector2& r0, vector3& local_E, vector3& local_B) const override;

 private:
  const v3f& E_;
  const v3f& B_;
};

#endif // SIMPLE_INTERPOLATION_HPP
