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
	vector3_field(int size) : size_x_(size), size_y_(size) {
		field_ = new vector3[size_x_*size_y_];
	};
	~vector3_field() {
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


//-------------------------------------------------------------------------------------------------

class periodic_vector3_field : public vector3_field {
public:
	periodic_vector3_field(int size_x, int size_y) : vector3_field(size_x, size_y) {};
	periodic_vector3_field(int size) : vector3_field(size) {};

	int begin_x(Axis field_component) const override;
	int begin_y(Axis field_component) const override;
	int end_x(Axis field_component) const override;
	int end_y(Axis field_component) const override;

private:
	vector3& field_at(int i, int j) override;
	vector3 field_at(int i, int j) const override;
};

class reflecting_vector3_field : public vector3_field {
public:
	reflecting_vector3_field(int size_x, int size_y) : vector3_field(size_x, size_y) {};
	reflecting_vector3_field(int size) : vector3_field(size) {};

private:
	vector3 zero_;

	vector3& field_at(int i, int j) override;
	vector3 field_at(int i, int j) const override;
};

//________________________________________________________________________

class reflecting_Electric_field : public reflecting_vector3_field {
public:
	reflecting_Electric_field(int size_x, int size_y)
	: reflecting_vector3_field(size_x, size_y) {};
	reflecting_Electric_field(int size)
	: reflecting_vector3_field(size) {};
	
	int begin_x(Axis field_component) const override;
	int begin_y(Axis field_component) const override;
	int end_x(Axis field_component) const override;
	int end_y(Axis field_component) const override;
};

//________________________________________________________________________

class reflecting_Magnetic_field : public reflecting_vector3_field {
public:
	reflecting_Magnetic_field(int size_x, int size_y)
	: reflecting_vector3_field(size_x, size_y) {};
	reflecting_Magnetic_field(int size)
	: reflecting_vector3_field(size) {};
	
	int begin_x(Axis field_component) const override;
	int begin_y(Axis field_component) const override;
	int end_x(Axis field_component) const override;
	int end_y(Axis field_component) const override;
};

//-------------------------------------------------------------------------------------------------

//#################################################################################################

#endif