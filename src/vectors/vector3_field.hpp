#ifndef SRC_VECTORS_VECTOR3_FIELD_HPP
#define SRC_VECTORS_VECTOR3_FIELD_HPP

#include "src/pch.h"
#include "vector_classes.hpp"

/**
 * @brief This class provides an interface for a vector field.
 * It proxies indexing at the border, so simple boundary
 * conditions automatically works.
 * 
 * @todo Refactor this class to adopting strategies.
 * @todo Replace vector<vector3> and get_vector
 * by vector<double> and indexing_impl.
 */
class vector3_field {
public:
	vector3_field() = default;

	vector3_field(int size_x, int size_y);

	virtual ~vector3_field() = default;

	int size_x() const { return size_x_; }
	int size_y() const { return size_y_; }

	virtual int ix_first (Axis) const = 0;
	virtual int iy_first (Axis) const = 0;
	virtual int ix_last	 (Axis) const = 0;
	virtual int iy_last	 (Axis) const = 0;

	double& x(int ny, int nx);
	double& y(int ny, int nx);
	double& z(int ny, int nx);

	double x(int ny, int nx) const;
	double y(int ny, int nx) const;
	double z(int ny, int nx) const;

protected:
	int size_x_, size_y_;
	std::vector<vector3> field_;

	virtual vector3& get_vector(Axis, int ny, int nx) = 0;
	virtual const vector3&  get_vector(int ny, int nx) const = 0;
};

class px_py_vector3_field : public vector3_field {
public:
	px_py_vector3_field(int size_x, int size_y)
		: vector3_field(size_x, size_y) {};

	int ix_first (Axis) const override;
	int iy_first (Axis) const override;
	int ix_last	 (Axis) const override;
	int iy_last	 (Axis) const override;

private:
	vector3& get_vector(Axis axis, int ny, int nx) override;
	const vector3&  get_vector(int ny, int nx) const override;
};

class rx_ry_vector3_field : public vector3_field {
public:
	rx_ry_vector3_field(std::string type, int size_x, int size_y);

	int ix_first (Axis) const override;
	int iy_first (Axis) const override;
	int ix_last	 (Axis) const override;
	int iy_last	 (Axis) const override;

private:
	enum Boundaries { left=3*0, right=3*1, upper=3*2, lower=3*3 };
	double bound[3*4];
	
	vector3 zero_ = {0, 0, 0};

	vector3& get_vector(Axis axis, int ny, int nx) override;
	const vector3&  get_vector(int ny, int nx) const override;
};

#endif  // SRC_VECTORS_VECTOR3_FIELD_HPP
