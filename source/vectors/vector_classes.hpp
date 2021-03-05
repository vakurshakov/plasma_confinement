#ifndef VECTOR_CLASSES_H
#define VECTOR_CLASSES_H

//#################################################################################################

enum Axes { XY, YZ, XZ };

class vector2 {
public:	
	vector2() : x_(0), y_(0) {};
	vector2(double x, double y) : x_(x), y_(y) {};
	
	double& x() { return x_; };
	double& y() { return y_; };
	double x() const { return x_; };
	double y() const { return y_; };

	vector2& operator+=(const vector2& other);
	vector2 operator/(const double a) const;
	vector2 operator*(const double a) const;
	
private:
	double x_; 
	double y_;
};

class vector3 {
public:
	vector3() : x_(0), y_(0), z_(0) {};
	vector3(double x, double y, double z) : x_(x), y_(y), z_(z) {};
	
	double& x() { return x_; };
	double& y() { return y_; };
	double& z() { return z_; };
	double x() const { return x_; };
	double y() const { return y_; };
	double z() const { return z_; };

	double dot(const vector3 &other) const;
	vector3 cross(const vector3 &other) const;
	vector3 operator+(const vector3& other) const;
	vector3& operator+=(const vector3& other);
	vector3 operator*(const double a) const;
	vector3 operator/(const double a) const;
	vector2 squeeze(const Axes axes) const;

	

private:
	double x_; 
	double y_;
	double z_;
};

//#################################################################################################

#endif
