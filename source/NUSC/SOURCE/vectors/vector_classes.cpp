#include "./vector_classes.hpp"

vector2& vector2::operator+=(const vector2& other)
{
	x += other.x;
	y += other.y;
	return *this;
}

vector2 vector2::operator/(const double a) const
{
	vector2 temp;

	temp.x = x/a;
	temp.y = y/a;
		
	return temp;
}

vector2 vector2::operator*(const double a) const
{
	vector2 temp;

	temp.x = x*a;
	temp.y = y*a;

	return temp;
}


double vector3::dot(const vector3 &other) const
{ 
	return x*other.x + y*other.y + z*other.z;
}

vector3 vector3::cross(const vector3 &other) const
{
		vector3 temp;

		temp.x = + ( y*other.z - z*other.y );
		temp.y = - ( x*other.z - z*other.x );
		temp.z = + ( x*other.y - y*other.x );

		return temp;		
}

vector3 vector3::operator+(const vector3& other) const
{
	vector3 temp;

	temp.x = x + other.x;
	temp.y = y + other.y;
	temp.z = z + other.z;

	return temp;
}
	
vector3& vector3::operator+=(const vector3& other) {
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

vector3 vector3::operator*(const double a) const
{
	vector3 temp;

	temp.x = x*a;
	temp.y = y*a;
	temp.z = z*a;

	return temp;
}

vector3 vector3::operator/(const double a) const
{
	vector3 temp;

	temp.x = x/a;
	temp.y = y/a;
	temp.z = z/a;

	return temp;
}

vector2 vector3::squeeze(const Axes axes) const
{
	vector2 temp;
	
	switch (axes) {
		case XY:
			temp.x = x;
			temp.y = y;	 
			break;
		case YZ: 
			temp.x = y;
			temp.y = z;	 
			break;		
		case XZ:
			temp.x = x;
			temp.y = z;	 
			break;
		default:
			throw("bad index");
			break;
	}
	
	return temp;
}


