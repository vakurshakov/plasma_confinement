#ifndef VECTOR_CLASSES_H
#define VECTOR_CLASSES_H

//#################################################################################################

enum Axes { XY, YZ, XZ };

class vector2 {
public:	
	vector2() : x(0), y(0) {};
	vector2(double _x, double _y) : x(_x), y(_y) {};

	vector2& operator+=(const vector2& other);
	vector2 operator/(const double a) const;
	vector2 operator*(const double a) const;
	
	double x; 
	double y;
};

class vector3 {
public:
	vector3() : x(0), y(0), z(0) {};
	vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {};

	double dot(const vector3 &other) const;
	vector3 cross(const vector3 &other) const;
	vector3 operator+(const vector3& other) const;
	vector3& operator+=(const vector3& other);
	vector3 operator*(const double a) const;
	vector3 operator/(const double a) const;
	vector2 squeeze(const Axes axes) const;

	double x; 
	double y;
	double z;
};

//#################################################################################################

#endif
