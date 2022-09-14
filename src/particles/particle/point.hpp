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
  Point() = delete;

  Point(const vector2& r, const vector3& p)
    : r(r), p(p) {}

  Point(vector2&& r, vector3&& p)
    : r(std::move(r)), p(std::move(p)) {}

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
void reflective_Xboundary(Point&, double size_x);
void reflective_Yboundary(Point&, double size_y);
void periodic_Xboundary(Point&, double size_x);
void periodic_Yboundary(Point&, double size_y);

#endif  // SRC_PARTICLES_POINT_HPP
