#ifndef PARTICLE_H
#define PARTICLE_H

//#################################################################################################

#include "../vectors/vector_classes.hpp"
#include "../vectors/vector3_field.hpp"


class Particle {
public:
	Particle(): r_(), p_() {};
	Particle(vector2& r, vector3& p): r_(r), p_(p) {};
	
	vector2& r() { return r_; };
	double& x() { return r_.x(); };
	double& y() { return r_.y(); };
	
	vector2 r() const { return r_; };
	double x() const { return r_.x(); };
	double y() const { return r_.y(); };

	vector3& p() { return p_; };
	double& px() { return p_.x(); };
	double& py() { return p_.y(); };
	double& pz() { return p_.z(); };
	
	vector3 p() const { return p_; };
	double px() const { return p_.x(); };
	double py() const { return p_.y(); };
	double pz() const { return p_.z(); };

private:
	vector2 r_;
	vector3 p_;
};


void reflective_Xboundary(Particle& particle, double SIZE_X);
void reflective_Yboundary(Particle& particle, double SIZE_Y);
void periodic_Xboundary(Particle& particle, double SIZE_X);
void periodic_Yboundary(Particle& particle, double SIZE_Y);

//#################################################################################################

#endif
