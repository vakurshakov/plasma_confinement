#ifndef POINT_H
#define POINT_H

//#################################################################################################

#include "../vectors/vector_classes.hpp"

class Point {
public:
	Point(): r_(), p_() {};
	Point(const vector2& r, const vector3& p): r_(r), p_(p) {};
	
	vector2& r() { return r_; };
	double& x() { return r_.x; };
	double& y() { return r_.y; };
	
	vector2 r() const { return r_; };
	double x() const { return r_.x; };
	double y() const { return r_.y; };

	vector3& p() { return p_; };
	double& px() { return p_.x; };
	double& py() { return p_.y; };
	double& pz() { return p_.z; };
	
	vector3 p() const { return p_; };
	double px() const { return p_.x; };
	double py() const { return p_.y; };
	double pz() const { return p_.z; };

private:
	vector2 r_;
	vector3 p_;
};


void reflective_Xboundary(Point* particle, double size_x);
void reflective_Yboundary(Point* particle, double size_y);
void periodic_Xboundary(Point* particle, double size_x);
void periodic_Yboundary(Point* particle, double size_y);

//#################################################################################################

#endif
