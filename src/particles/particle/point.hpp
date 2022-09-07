#ifndef POINT_H
#define POINT_H

//#################################################################################################

#include "../../vectors/vector_classes.hpp"


class Point {
public:
	// Default constructor.
	Point() = delete;
	
	/**
	 * @param r Radius vector of a point.
	 * @param p Momentum of a point.
	 */
	Point(const vector2& r, const vector3& p): r_(r), p_(p) {}

	vector2& r() { return r_; }
	double&  x() { return r_.x(); }
	double&  y() { return r_.y(); }
	
	const vector2& r() const { return r_; }
	double  x() const { return r_.x(); }
	double  y() const { return r_.y(); }

	vector3& p() { return p_; }
	double& px() { return p_.x(); }
	double& py() { return p_.y(); }
	double& pz() { return p_.z(); }
	
	const vector3& p() const { return p_; }
	double px() const { return p_.x(); }
	double py() const { return p_.y(); }
	double pz() const { return p_.z(); }

private:
	vector2 r_;
	vector3 p_;
};


void reflective_Xboundary(Point&, double size_x);
void reflective_Yboundary(Point&, double size_y);
void periodic_Xboundary(Point&, double size_x);
void periodic_Yboundary(Point&, double size_y);

//#################################################################################################

#endif