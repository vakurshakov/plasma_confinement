#ifndef CONCRETE_POINT_INTERPOLATION
#define CONCRETE_POINT_INTERPOLATION

//#################################################################################################

#include "abstract_strategies.hpp"

#include <memory>

#include "../vectors/vector_classes.hpp"


class Local_field_adder {
public:
    virtual ~Local_field_adder() = default;
    virtual const vector3& add_local_E(const vector2& r0) const = 0;
    virtual const vector3& add_local_B(const vector2& r0) const = 0;
};


class Homogenius_field_adder : public Local_field_adder {
public:
    Homogenius_field_adder(const vector3 E0, const vector3 B0) : E0_(E0), B0_(B0) {};
    const vector3& add_local_E(const vector2& r0) const override { return E0_; }
    const vector3& add_local_B(const vector2& r0) const override { return B0_; }

private:
    const vector3 E0_;
    const vector3 B0_;
};


class Concrete_point_interpolation : public Interpolation {
public:
    Concrete_point_interpolation(std::unique_ptr<Local_field_adder>);
    void process(const vector2& r0, vector3& local_E, vector3& local_B) const override;

private:
    const std::unique_ptr<Local_field_adder> adder_;
}; 


//#################################################################################################

#endif // CONCRETE_POINT_INTERPOLATION