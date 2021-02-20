#ifndef VECTOR_CLASSES_H
#define VECTOR_CLASSES_H
	#include "vector_classes.hpp"
#endif

#include <fstream>

enum Axis { X, Y, Z };

class vector3_field {
public:
	vector3_field(int SIZE_X, int SIZE_Y) : msize_x(SIZE_X), msize_y(SIZE_Y) {
		field = new vector3[msize_x*msize_y];
	};
	vector3_field(int SIZE) : msize_x(SIZE), msize_y(SIZE) {
		field = new vector3[msize_x*msize_y];
	};
	~vector3_field() {
		delete[] field;
	}

	int size_x() const { return msize_x; }
	int size_y() const { return msize_y; }
	virtual int end_x(Axis field_component) const = 0;
	virtual int end_y(Axis field_component) const = 0;

	double& x(int i, int j) { return field_at(i,j).x(); }
	double& y(int i, int j) { return field_at(i,j).y(); }
	double& z(int i, int j) { return field_at(i,j).z(); }
	
	double x(int i, int j) const { return field_at(i,j).x(); }
	double y(int i, int j) const { return field_at(i,j).y(); }
	double z(int i, int j) const { return field_at(i,j).z(); }

/*
NOTE: можно сделать вот такую реализацию, где E(x,y).x, если для класса vector2(3) убрать инкапсуляцию

	vector3& operator(int i, int j) = 0;
	vector3 operator(int i, int j) const = 0; 
*/

protected:
	int msize_x, msize_y;
	vector3* field;
	
	virtual vector3& field_at(int i, int j) = 0;
	virtual vector3 field_at(int i, int j) const = 0;
};

class reflective_vector3_field : public vector3_field {
public:
	reflective_vector3_field(int SIZE_X, int SIZE_Y) : vector3_field(SIZE_X, SIZE_Y) {};
	reflective_vector3_field(int SIZE) : vector3_field(SIZE) {};
	
	int end_x(Axis field_component) const override;
	int end_y(Axis field_component) const override;

private:
	vector3 zero;

	vector3& field_at(int i, int j) override;
	vector3 field_at(int i, int j) const override;
};

class periodic_vector3_field : public vector3_field {
public:
	periodic_vector3_field(int SIZE_X, int SIZE_Y) : vector3_field(SIZE_X, SIZE_Y) {};
	periodic_vector3_field(int SIZE) : vector3_field(SIZE) {};

	int end_x(Axis field_component) const override;
	int end_y(Axis field_component) const override;

private:
	vector3& field_at(int i, int j) override;
	vector3 field_at(int i, int j) const override;
};

class rh_pv_vector3_field : public vector3_field {
public:
	rh_pv_vector3_field(int SIZE_X, int SIZE_Y) : vector3_field(SIZE_X, SIZE_Y) {};
	rh_pv_vector3_field(int SIZE) : vector3_field(SIZE) {};

	int end_x(Axis field_component) const override;
	int end_y(Axis field_component) const override;

private:
	vector3 zero;

	vector3& field_at(int i, int j) override;
	vector3 field_at(int i, int j) const override;
};

class rv_ph_vector3_field : public vector3_field {
public:
	rv_ph_vector3_field(int SIZE_X, int SIZE_Y) : vector3_field(SIZE_X, SIZE_Y) {};
	rv_ph_vector3_field(int SIZE) : vector3_field(SIZE) {};

	int end_x(Axis field_component) const override;
	int end_y(Axis field_component) const override;

private:
	vector3 zero;

	vector3& field_at(int i, int j) override;
	vector3 field_at(int i, int j) const override;
};