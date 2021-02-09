#include "vector_classes.hpp"

vector2& vector2::operator+=(const vector2& other)
{
	m_x += other.x();
	m_y += other.y();
	return *this;
}

vector2 vector2::operator/(const double a) const
{
	vector2 temp;

	temp.x() = m_x/a;
	temp.y() = m_y/a;
		
	return temp;
}

vector2 vector2::operator*(const double a) const
{
	vector2 temp;

	temp.x() = m_x*a;
	temp.y() = m_y*a;

	return temp;
}


double vector3::dot(const vector3 &other) const
{ 
	return m_x*other.x() + m_y*other.y() + m_z*other.z();
}

vector3 vector3::cross(const vector3 &other) const
{
		vector3 temp;

		temp.x() = + ( m_y*other.z() - m_z*other.y() );
		temp.y() = - ( m_x*other.z() - m_z*other.x() );
		temp.z() = + ( m_x*other.y() - m_y*other.x() );

		return temp;		
}

vector3 vector3::operator+(const vector3& other) const
{
	vector3 temp;

	temp.x() = m_x + other.x();
	temp.y() = m_y + other.y();
	temp.z() = m_z + other.z();

	return temp;
}
	
vector3& vector3::operator+=(const vector3& other) {
	m_x += other.x();
	m_y += other.y();
	m_z += other.z();
	return *this;
}

vector3 vector3::operator*(const double a) const
{
	vector3 temp;

	temp.x() = m_x*a;
	temp.y() = m_y*a;
	temp.z() = m_z*a;

	return temp;
}

vector3 vector3::operator/(const double a) const
{
	vector3 temp;

	temp.x() = m_x/a;
	temp.y() = m_y/a;
	temp.z() = m_z/a;

	return temp;
}

vector2 vector3::squeeze(const Axes axes) const
{
	vector2 temp;
	
	switch (axes) {
		case XY:
			temp.x() = m_x;
			temp.y() = m_y;	 
			break;
		case YZ: 
			temp.x() = m_y;
			temp.y() = m_z;	 
			break;		
		case XZ:
			temp.x() = m_x;
			temp.y() = m_z;	 
			break;
		default:
			throw("bad index");
			break;
	}
	
	return temp;
}


