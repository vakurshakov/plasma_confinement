#ifndef VECTOR3_FIELD_H
#define VECTOR3_FIELD_H

//#################################################################################################

#include "./vector_classes.hpp"


enum Axis { X, Y, Z };

class vector3_field {
public:
	vector3_field() = default;

	vector3_field(int size_x, int size_y) : size_x_(size_x), size_y_(size_y) {
		field_ = new vector3[size_x_*size_y_];
	};

	virtual ~vector3_field() {
		delete[] field_;
	}

	int size_x() const { return size_x_; }
	int size_y() const { return size_y_; }
	
	virtual int begin_x(Axis field_component) const = 0;
	virtual int begin_y(Axis field_component) const = 0;
	virtual int end_x(Axis field_component) const = 0;
	virtual int end_y(Axis field_component) const = 0;

	double& x(int i, int j) { return field_at(i,j).x; }
	double& y(int i, int j) { return field_at(i,j).y; }
	double& z(int i, int j) { return field_at(i,j).z; }
	
	double x(int i, int j) const { return field_at(i,j).x; }
	double y(int i, int j) const { return field_at(i,j).y; }
	double z(int i, int j) const { return field_at(i,j).z; }

protected:
	int size_x_, size_y_;
	vector3* field_;
	
	virtual vector3& field_at(int i, int j) = 0;
	virtual vector3 field_at(int i, int j) const = 0;
};


//_________________________________________________________________________________________________

class px_py_vector3_field : public vector3_field {
public:
	px_py_vector3_field(int size_x, int size_y)
		: vector3_field(size_x, size_y) {};

	int begin_x(Axis field_component) const override;
	int begin_y(Axis field_component) const override;
	int end_x(Axis field_component) const override;
	int end_y(Axis field_component) const override;

private:
	vector3& field_at(int i, int j) override;
	vector3 field_at(int i, int j) const override;
};

//-----------------------------------------------------------------------

class rx_ry_vector3_field : public vector3_field {
public:
	rx_ry_vector3_field(int size_x, int size_y)
		: vector3_field(size_x, size_y) {};
	
private:
	vector3 zero_;

	vector3& field_at(int i, int j) override;
	vector3 field_at(int i, int j) const override;
};


class rx_ry_Electric_field : public rx_ry_vector3_field {
public:
	rx_ry_Electric_field(int size_x, int size_y)
		: rx_ry_vector3_field(size_x, size_y) {};
	
	int begin_x(Axis field_component) const override;
	int begin_y(Axis field_component) const override;
	int end_x(Axis field_component) const override;
	int end_y(Axis field_component) const override;
};


class rx_ry_Magnetic_field : public rx_ry_vector3_field {
public:
	rx_ry_Magnetic_field(int size_x, int size_y)
		: rx_ry_vector3_field(size_x, size_y) {};
	
	int begin_x(Axis field_component) const override;
	int begin_y(Axis field_component) const override;
	int end_x(Axis field_component) const override;
	int end_y(Axis field_component) const override;
};

//-----------------------------------------------------------------------

class px_ry_vector3_field : public vector3_field {
public:
	px_ry_vector3_field(int size_x, int size_y)
		: vector3_field(size_x, size_y) {};
	
private:
	vector3 zero_;

	vector3& field_at(int i, int j) override;
	vector3 field_at(int i, int j) const override;
};

class px_ry_Electric_field : public px_ry_vector3_field {
public:
	px_ry_Electric_field(int size_x, int size_y)
		: px_ry_vector3_field(size_x, size_y) {};
	
	int begin_x(Axis field_component) const override;
	int begin_y(Axis field_component) const override;
	int end_x(Axis field_component) const override;
	int end_y(Axis field_component) const override;
};


class px_ry_Magnetic_field : public px_ry_vector3_field {
public:
	px_ry_Magnetic_field(int size_x, int size_y)
		: px_ry_vector3_field(size_x, size_y) {};
	
	int begin_x(Axis field_component) const override;
	int begin_y(Axis field_component) const override;
	int end_x(Axis field_component) const override;
	int end_y(Axis field_component) const override;
};

//-----------------------------------------------------------------------

class rx_py_vector3_field : public vector3_field {
public:
	rx_py_vector3_field(int size_x, int size_y)
		: vector3_field(size_x, size_y) {};
	
private:
	vector3 zero_;

	vector3& field_at(int i, int j) override;
	vector3 field_at(int i, int j) const override;
};

class rx_py_Electric_field : public rx_py_vector3_field {
public:
	rx_py_Electric_field(int size_x, int size_y)
		: rx_py_vector3_field(size_x, size_y) {};
	
	int begin_x(Axis field_component) const override;
	int begin_y(Axis field_component) const override;
	int end_x(Axis field_component) const override;
	int end_y(Axis field_component) const override;
};


class rx_py_Magnetic_field : public rx_py_vector3_field {
public:
	rx_py_Magnetic_field(int size_x, int size_y)
		: rx_py_vector3_field(size_x, size_y) {};
	
	int begin_x(Axis field_component) const override;
	int begin_y(Axis field_component) const override;
	int end_x(Axis field_component) const override;
	int end_y(Axis field_component) const override;
};


//_________________________________________________________________________________________________

//#################################################################################################

#endif