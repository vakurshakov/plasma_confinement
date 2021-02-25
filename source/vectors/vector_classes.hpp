#ifndef VECTOR_CLASSES_H
#define VECTOR_CLASSES_H

//#################################################################################################

enum Axes { XY, YZ, XZ };

class vector2 {
public:	
	vector2() : m_x(0), m_y(0) {};
	vector2(double x, double y) : m_x(x), m_y(y) {};
	
	double& x() { return m_x; };
	double& y() { return m_y; };
	double x() const { return m_x; };
	double y() const { return m_y; };

	vector2& operator+=(const vector2& other);
	vector2 operator/(const double a) const;
	vector2 operator*(const double a) const;
	
private:
	double m_x; 
	double m_y;
};

class vector3 {
public:
	vector3() : m_x(0), m_y(0), m_z(0) {};
	vector3(double x, double y, double z) : m_x(x), m_y(y), m_z(z) {};
	
	double& x() { return m_x; };
	double& y() { return m_y; };
	double& z() { return m_z; };
	double x() const { return m_x; };
	double y() const { return m_y; };
	double z() const { return m_z; };

	double dot(const vector3 &other) const;
	vector3 cross(const vector3 &other) const;
	vector3 operator+(const vector3& other) const;
	vector3& operator+=(const vector3& other);
	vector3 operator*(const double a) const;
	vector3 operator/(const double a) const;
	vector2 squeeze(const Axes axes) const;

	

private:
	double m_x; 
	double m_y;
	double m_z;
};

//#################################################################################################

#endif
