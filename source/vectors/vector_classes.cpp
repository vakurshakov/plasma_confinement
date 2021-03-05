#include "./vector_classes.hpp"

vector2& vector2::operator+=(const vector2& other)
{
	x_ += other.x();
	y_ += other.y();
	return *this;
}

vector2 vector2::operator/(const double a) const
{
	vector2 temp;

	temp.x() = x_/a;
	temp.y() = y_/a;
		
	return temp;
}

vector2 vector2::operator*(const double a) const
{
	vector2 temp;

	temp.x() = x_*a;
	temp.y() = y_*a;

	return temp;
}


double vector3::dot(const vector3 &other) const
{ 
	return x_*other.x() + y_*other.y() + z_*other.z();
}

vector3 vector3::cross(const vector3 &other) const
{
		vector3 temp;

		temp.x() = + ( y_*other.z() - z_*other.y() );
		temp.y() = - ( x_*other.z() - z_*other.x() );
		temp.z() = + ( x_*other.y() - y_*other.x() );

		return temp;		
}

vector3 vector3::operator+(const vector3& other) const
{
	vector3 temp;

	temp.x() = x_ + other.x();
	temp.y() = y_ + other.y();
	temp.z() = z_ + other.z();

	return temp;
}
	
vector3& vector3::operator+=(const vector3& other) {
	x_ += other.x();
	y_ += other.y();
	z_ += other.z();
	return *this;
}

vector3 vector3::operator*(const double a) const
{
	vector3 temp;

	temp.x() = x_*a;
	temp.y() = y_*a;
	temp.z() = z_*a;

	return temp;
}

vector3 vector3::operator/(const double a) const
{
	vector3 temp;

	temp.x() = x_/a;
	temp.y() = y_/a;
	temp.z() = z_/a;

	return temp;
}

vector2 vector3::squeeze(const Axes axes) const
{
	vector2 temp;
	
	switch (axes) {
		case XY:
			temp.x() = x_;
			temp.y() = y_;	 
			break;
		case YZ: 
			temp.x() = y_;
			temp.y() = z_;	 
			break;		
		case XZ:
			temp.x() = x_;
			temp.y() = z_;	 
			break;
		default:
			throw("bad index");
			break;
	}
	
	return temp;
}


