#ifndef VECTOR3_FIELD_H
#define VECTOR3_FIELD_H

//#################################################################################################

#include <vector>
#include "vector_classes.hpp"

enum class Field_axis { Fx, Fy, Fz };

class vector3_field {
public:
	vector3_field() = default;

	vector3_field(int size_x, int size_y) : size_x_(size_x), size_y_(size_y) {
		field_.reserve(size_x_*size_y_);
	};

	virtual ~vector3_field() = default;

	int size_x() const { return size_x_; }
	int size_y() const { return size_y_; }

	virtual int ix_first (Field_axis) const = 0;
	virtual int iy_first (Field_axis) const = 0;
	virtual int ix_last	 (Field_axis) const = 0;
	virtual int iy_last	 (Field_axis) const = 0;

	virtual double& x(int ny, int nx) = 0;
	virtual double& y(int ny, int nx) = 0;
	virtual double& z(int ny, int nx) = 0;
	virtual double  x(int ny, int nx) const = 0;
	virtual double  y(int ny, int nx) const = 0;
	virtual double  z(int ny, int nx) const = 0;

protected:
	int size_x_, size_y_;
	std::vector<vector3> field_;

	virtual vector3& get_vector(Field_axis, int ny, int nx) = 0;
	virtual const vector3&  get_vector(Field_axis, int ny, int nx) const = 0;
};


//_________________________________________________________________________________________________

class px_vector3_field : public virtual vector3_field {
public:
	px_vector3_field() = default;

	int ix_first (Field_axis) const override { return 0; };
	int ix_last	 (Field_axis) const override { return size_x_; };
};

class py_vector3_field : public virtual vector3_field {
public:
	py_vector3_field() = default;

	int iy_first (Field_axis) const override { return 0; };
	int iy_last	 (Field_axis) const override { return size_y_; };
};

class rx_vector3_field : public virtual vector3_field {
public:
	rx_vector3_field() = default;

	int ix_first (Field_axis) const override;
	int ix_last	 (Field_axis) const override;
};

class ry_vector3_field : public virtual vector3_field {
public:
	ry_vector3_field() = default;

	int iy_first (Field_axis) const override;
	int iy_last	 (Field_axis) const override;
};

using p_v3f = px_py_vector3_field;

class px_py_vector3_field : public px_vector3_field, public py_vector3_field {
public:
	px_py_vector3_field(int size_x, int size_y)
		: vector3_field(size_x, size_y) {};
	
	double& x(int ny, int nx)	    override { get_vector(Field_axis::Fx, ny, nx).x; };
	double& y(int ny, int nx)	    override { get_vector(Field_axis::Fy, ny, nx).y; };
	double& z(int ny, int nx)	    override { get_vector(Field_axis::Fz, ny, nx).z; };
	double  x(int ny, int nx) const override { get_vector(Field_axis::Fx, ny, nx).x; };
	double  y(int ny, int nx) const override { get_vector(Field_axis::Fy, ny, nx).y; };
	double  z(int ny, int nx) const override { get_vector(Field_axis::Fz, ny, nx).z; };

private:
	vector3& get_vector(Field_axis axis, int ny, int nx) override;
	const vector3&  get_vector(Field_axis, int ny, int nx) const override;
};

const vector3& px_py_vector3_field::get_vector(Field_axis axis, int ny, int nx) const {
	if (nx < ix_first(axis) && ny < iy_first(axis)) {
		return field_[(size_y_ + ny%size_y_)*size_x_ + (size_x_ + nx%size_x_)]; 
	}
	else if (nx < ix_first(axis)) {
		return field_[(ny%size_y_)*size_x_ + (size_x_ + nx%size_x_)]; 
	}
	else if (ny < iy_first(axis)) {
		return field_[(size_y_ + ny%size_y_)*size_x_ + nx%size_x_]; 
	}
	else return field_[(ny%size_y_)*size_x_ + nx%size_x_];
}

vector3& px_py_vector3_field::get_vector(Field_axis axis, int ny, int nx) {
	return const_cast<vector3&>(static_cast<const p_v3f&>(*this).get_vector(axis, ny, nx));
}

/* 
//-----------------------------------------------------------------------

class rx_ry_vector3_field : public vector3_field {
public:
	rx_ry_vector3_field(int size_x, int size_y)
		: vector3_field(size_x, size_y) {};

	int ix_first (Field_axis) const override;
	int iy_first (Field_axis) const override;
	int ix_last	 (Field_axis) const override;
	int iy_last	 (Field_axis) const override;

	double& x(int ny, int nx)		override;
	double& y(int ny, int nx)		override;
	double& z(int ny, int nx)		override;
	double  x(int ny, int nx) const override;
	double  y(int ny, int nx) const override;
	double  z(int ny, int nx) const override;

protected:
	vector3 zero_;
};


class rx_ry_Electric_field : public rx_ry_vector3_field {
public:
	rx_ry_Electric_field(int size_x, int size_y)
		: rx_ry_vector3_field(size_x, size_y) {};
	
	int ix_first (Field_axis) const override;
	int iy_first (Field_axis) const override;
	int ix_last	 (Field_axis) const override;
	int iy_last	 (Field_axis) const override;

	double& x(int ny, int nx)		override;
	double& y(int ny, int nx)		override;
	double& z(int ny, int nx)		override;
	double  x(int ny, int nx) const override;
	double  y(int ny, int nx) const override;
	double  z(int ny, int nx) const override;
};


class rx_ry_Magnetic_field : public rx_ry_vector3_field {
public:
	rx_ry_Magnetic_field(int size_x, int size_y)
		: rx_ry_vector3_field(size_x, size_y) {};
	
	int ix_first (Field_axis) const override;
	int iy_first (Field_axis) const override;
	int ix_last	 (Field_axis) const override;
	int iy_last	 (Field_axis) const override;

	double& x(int ny, int nx)		override;
	double& y(int ny, int nx)		override;
	double& z(int ny, int nx)		override;
	double  x(int ny, int nx) const override;
	double  y(int ny, int nx) const override;
	double  z(int ny, int nx) const override;
};

//-----------------------------------------------------------------------

class px_ry_vector3_field : public vector3_field {
public:
	px_ry_vector3_field(int size_x, int size_y)
		: vector3_field(size_x, size_y) {};

	int ix_first (Field_axis) const override;
	int iy_first (Field_axis) const override;
	int ix_last	 (Field_axis) const override;
	int iy_last	 (Field_axis) const override;

	double& x(int ny, int nx)		override;
	double& y(int ny, int nx)		override;
	double& z(int ny, int nx)		override;
	double  x(int ny, int nx) const override;
	double  y(int ny, int nx) const override;
	double  z(int ny, int nx) const override;

protected:
	vector3 zero_;
};

class px_ry_Electric_field : public px_ry_vector3_field {
public:
	px_ry_Electric_field(int size_x, int size_y)
		: px_ry_vector3_field(size_x, size_y) {};
	
	int ix_first (Field_axis) const override;
	int iy_first (Field_axis) const override;
	int ix_last	 (Field_axis) const override;
	int iy_last	 (Field_axis) const override;

	double& x(int ny, int nx)		override;
	double& y(int ny, int nx)		override;
	double& z(int ny, int nx)		override;
	double  x(int ny, int nx) const override;
	double  y(int ny, int nx) const override;
	double  z(int ny, int nx) const override;
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
	vector3 zero_;};

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
 */
//#################################################################################################

#endif