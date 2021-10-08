#include "vector_classes.hpp"

//_______ vector2 methods _________________________________________________________________________

vector2& vector2::operator+=(const vector2& other)
{
	vec[X] += other.x();
	vec[Y] += other.y();
	return *this;
}

vector2 vector2::operator*(double a) const  {
	return { vec[X]*a, vec[Y]*a };
}

vector2 vector2::operator/(double a) const {
	return this->operator*(1./a);
}

//_______ vector3 methods _________________________________________________________________________

double vector3::dot(const vector3 &other) const { 
	return vec[X]*other.x() + vec[Y]*other.y() + vec[Z]*other.z();
}

vector3 vector3::cross(const vector3 &other) const {
	return {+ (vec[Y]*other.z() - vec[Z]*other.y()),
			- (vec[X]*other.z() - vec[Z]*other.x()),
			+ (vec[X]*other.y() - vec[Y]*other.x()) };		
}

vector3 vector3::operator+(const vector3& other) const {
	return { vec[X] + other.x(), vec[Y] + other.y(), vec[Z] + other.z() };
}
	
vector3& vector3::operator+=(const vector3& other)
{
	vec[X] += other.x();
	vec[Y] += other.y();
	vec[Z] += other.z();

	return *this;
}

vector3 vector3::operator*(double a) const {
	return { vec[X]*a, vec[Y]*a, vec[Z]*a };
}

vector3 vector3::operator/(double a) const {
	return this->operator*(1./a);
}

vector2 vector3::squeeze(const Axes axes) const
{
	vector2 temp;

	switch (axes) {
		case XY:
			temp = {vec[X], vec[Y]};
			break;
		case YZ: 
			temp = {vec[Y], vec[Z]};	 
			break;		
		case XZ:
			temp = {vec[X], vec[Z]};	 
			break;
	}
	
	return temp;
}


