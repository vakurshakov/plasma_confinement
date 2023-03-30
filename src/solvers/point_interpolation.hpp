#ifndef POINT_INTERPOLATION_HPP
#define POINT_INTERPOLATION_HPP

#include "abstract_strategies.hpp"

#include "src/pch.h"
#include "src/vectors/vector_classes.hpp"

class Local_field_adder {
 public:
  virtual ~Local_field_adder() = default;
  virtual const vector3& add_local_E(const vector2& r0) const = 0;
  virtual const vector3& add_local_B(const vector2& r0) const = 0;
};

class Const_field_adder : public Local_field_adder {
 public:
  Const_field_adder(const vector3& E0, const vector3& B0) : E0_(E0), B0_(B0) {}
  const vector3& add_local_E(const vector2& r0) const override { return E0_; }
  const vector3& add_local_B(const vector2& r0) const override { return B0_; }

 private:
  const vector3 E0_;
  const vector3 B0_;
};

class Point_interpolation : public Interpolation {
 public:
  Point_interpolation(std::unique_ptr<Local_field_adder>&& adder)
    : adder_(std::move(adder)) {}

  void process(const vector2& r0, vector3& local_E, vector3& local_B) const override {
    local_E += adder_->add_local_E(r0);
    local_B += adder_->add_local_B(r0);
  }

 private:
  const std::unique_ptr<Local_field_adder> adder_;
};

#endif // POINT_INTERPOLATION_HPP
