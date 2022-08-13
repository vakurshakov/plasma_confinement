#include "concrete_point_interpolation.hpp"

#include "src/pch.h"
#include "../vectors/vector_classes.hpp"


Concrete_point_interpolation
    ::Concrete_point_interpolation(std::unique_ptr<Local_field_adder> adder)
    :   adder_(std::move(adder)) {};

void Concrete_point_interpolation
    ::process(const vector2& r0, vector3& local_E, vector3& local_B) const
{
    local_E += adder_->add_local_E(r0);
    local_B += adder_->add_local_B(r0);
}