#ifndef SRC_PARTICLES_POINT_HPP
#define SRC_PARTICLES_POINT_HPP

#include "src/pch.h"
#include "src/vectors/vector_classes.hpp"

/**
 * @brief Small storage of the kinetic part of a particle. 
 * 
 * @param r Radius vector of a point.
 * @param p Momentum of a point.
 */
struct Point {
  Point() = default;

  template<class R = vector2, class P = vector3>
  Point(R&& r, P&& p)
    : r{std::forward<R>(r)}, p{std::forward<P>(p)} {}

  inline double& x() { return r.x(); }
  inline double& y() { return r.y(); }

  inline double x() const { return r.x(); }
  inline double y() const { return r.y(); }

  inline double& px() { return p.x(); }
  inline double& py() { return p.y(); }
  inline double& pz() { return p.z(); }

  inline double px() const { return p.x(); }
  inline double py() const { return p.y(); }
  inline double pz() const { return p.z(); }

  vector2 r;
  vector3 p;
};

// point-boundary interation
void reflective_x(Point&, double left, double right);
void reflective_y(Point&, double bottom, double top);
void periodic_x(Point&, double left, double right);
void periodic_y(Point&, double bottom, double top);

#endif  // SRC_PARTICLES_POINT_HPP
