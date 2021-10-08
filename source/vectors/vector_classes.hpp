#ifndef VECTOR_CLASSES_H
#define VECTOR_CLASSES_H

//#################################################################################################

#include <initializer_list>


enum Axis { X, Y, Z };
enum Axes { XY, YZ, XZ };

class vector2 {	
public:
	vector2() { 
		vec[X] = 0; vec[Y] = 0;
	}
	vector2(double x, double y) {
		vec[X] = x; vec[Y] = y;
	}

	double& x()	{ return vec[X]; }
	double& y()	{ return vec[Y]; }

	const double& x() const { return vec[X]; }
	const double& y() const { return vec[Y]; }

	vector2& operator+=(const vector2& other);
	vector2 operator*(double a) const;
	vector2 operator/(double a) const;
	
private:
	double vec[2];
};


class vector3 {
public:
	vector3() { 
		vec[0] = 0; vec[1] = 0; vec[2] = 0; 
	}
	vector3(double x, double y, double z) {
		vec[0] = x; vec[1] = y; vec[2] = z;
	}

	double& x() { return vec[X]; }	
	double& y() { return vec[Y]; }	
	double& z() { return vec[Z]; }

	const double& x() const { return vec[X]; }	
	const double& y() const { return vec[Y]; }	
	const double& z() const { return vec[Z]; }

	double 	 dot(const vector3 &other) const;
	vector3  cross(const vector3 &other) const;
	vector2  squeeze(const Axes axes) const;
	vector3  operator+(const vector3& other) const;
	vector3& operator+=(const vector3& other);
	vector3  operator*(double a) const;
	vector3  operator/(double a) const;

private:
	double vec[3];
};

//#################################################################################################

#endif
