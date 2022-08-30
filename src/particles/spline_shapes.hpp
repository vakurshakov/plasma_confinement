#ifndef SRC_PARTICLES_PARTICLES_SPLINE_SHAPES_HPP
#define SRC_PARTICLES_PARTICLES_SPLINE_SHAPES_HPP

#include "sort_common.hpp"

struct second_order_spline : public spline_shape {
  second_order_spline(): spline_shape(2) {}
  double form(double x, double dx) const override;
};

struct third_order_spline  : public spline_shape {
  third_order_spline(): spline_shape(2) {}
  double form(double x, double dx) const override;
};

struct fourth_order_spline : public spline_shape {
  fourth_order_spline(): spline_shape(3) {}
  double form(double x, double dx) const override;
};

struct fifth_order_spline  : public spline_shape {
  fifth_order_spline(): spline_shape(3) {}
  double form(double x, double dx) const override;
};

#endif  // SRC_PARTICLES_PARTICLES_SPLINE_SHAPES_HPP
