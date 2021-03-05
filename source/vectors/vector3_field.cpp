#include "./vector3_field.hpp"

// usefull functions:

int reflecting_x(const vector3_field& _F, Axis field_component)
{
	switch (field_component) {
		case X: return _F.size_x();
		case Y: return _F.size_x()-1;
		case Z: return _F.size_x()-1;
	}
	return 0;
}

int reflecting_y(const vector3_field& _F, Axis field_component)
{
	switch (field_component) {
		case X: return _F.size_y()-1;
		case Y: return _F.size_y();
		case Z: return _F.size_y()-1;
	}
	return 0;
}

int periodic_X(const vector3_field& _F, Axis field_component)
{
	switch (field_component) {
		case X: return _F.size_x();
		case Y: return _F.size_x();
		case Z: return _F.size_x();
	}
	return 0;
}

int periodic_Y(const vector3_field& _F, Axis field_component)
{
	switch (field_component) {
		case X: return _F.size_y();
		case Y: return _F.size_y();
		case Z: return _F.size_y();
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------

int reflective_vector3_field::end_x(Axis field_component) const
{ return reflecting_x(*this, field_component); }

int reflective_vector3_field::end_y(Axis field_component) const
{ return reflecting_y(*this, field_component); }

vector3& reflective_vector3_field::field_at(const int i, const int j)
{
	if ((i >= 1 && i < size_y_-1) && (j >= 1 && j < size_x_-1)){
		return field_[i*size_x_ + j]; 
	} else {
		zero_.x() = 0;
		zero_.y() = 0;
		zero_.z() = 0;
		return zero_;
	};
}

vector3 reflective_vector3_field::field_at(const int i, const int j) const
{
	if ((i >= 1 && i < size_y_-1) && (j >= 1 && j < size_x_-1)){
		return field_[i*size_x_ + j]; 
	} else {
		return zero_;
	};
}

//-------------------------------------------------------------------------------------------------

int periodic_vector3_field::end_x(Axis field_component) const
{ return periodic_X(*this, field_component); }

int periodic_vector3_field::end_y(Axis field_component) const
{ return periodic_Y(*this, field_component); }

vector3& periodic_vector3_field::field_at(const int i, const int j)
{
	if (i < 0 && j < 0) {
		return field_[(size_y_ + i%size_y_)*size_x_ + (size_x_ + j%size_x_)]; 
	} else if (i < 0) {
		return field_[(size_y_ + i%size_y_)*size_x_ + j%size_x_]; 
	} else if (j < 0) {
		return field_[(i%size_y_)*size_x_ + (size_x_ + j%size_x_)]; 
	} else return field_[(i%size_y_)*size_x_ + j%size_x_];
}

vector3 periodic_vector3_field::field_at(const int i, const int j) const
{
	if (i < 0 && j < 0) {
		return field_[(size_y_ + i%size_y_)*size_x_ + (size_x_ + j%size_x_)]; 
	} else if (i < 0) {
		return field_[(size_y_ + i%size_y_)*size_x_ + j%size_x_]; 
	} else if (j < 0) {
		return field_[(i%size_y_)*size_x_ + (size_x_ + j%size_x_)]; 
	} else return field_[(i%size_y_)*size_x_ + j%size_x_];
}

//-------------------------------------------------------------------------------------------------
// reflective horisontaly & periodic verticaly

int rh_pv_vector3_field::end_x(Axis field_component) const
{ return reflecting_x(*this, field_component); }

int rh_pv_vector3_field::end_y(Axis field_component) const
{ return periodic_Y(*this, field_component); }

vector3& rh_pv_vector3_field::field_at(const int i, const int j)
{
	if (i < 0 && (j >= 0 && j < size_x_)) {
		return field_[(size_y_ + i%size_y_)*size_x_ + j]; 
	} else if (j >= 0 && j < size_x_) {
		return field_[(i%size_y_)*size_x_ + j]; 
	} else {
		zero_.x() = 0;
		zero_.y() = 0;
		zero_.z() = 0;
		return zero_;
	};
}

vector3 rh_pv_vector3_field::field_at(const int i, const int j) const
{
	if (i < 0 && (j >= 0 && j < size_x_)) {
		return field_[(size_y_ + i%size_y_)*size_x_ + j]; 
	} else if (j >= 0 && j < size_x_) {
		return field_[(i%size_y_)*size_x_ + j]; 
	} else {
		return zero_;
	};
}

//-------------------------------------------------------------------------------------------------
//  reflective verticaly & periodic horisontaly

int rv_ph_vector3_field::end_x(Axis field_component) const
{ return periodic_X(*this, field_component); }

int rv_ph_vector3_field::end_y(Axis field_component) const
{ return reflecting_y(*this, field_component); }

vector3& rv_ph_vector3_field::field_at(const int i, const int j)
{
	if ((i >= 0 && i < size_y_) && j < 0) {
		return field_[i*size_x_ + (size_x_ + j%size_x_)]; 
	} else if (i >= 0 && i < size_y_) {
		return field_[i*size_x_ + j%size_x_]; 
	} else {
		zero_.x() = 0;
		zero_.y() = 0;
		zero_.z() = 0;
		return zero_;
	};
}

vector3 rv_ph_vector3_field::field_at(const int i, const int j) const
{
	if ((i >= 0 && i < size_y_) && j < 0) {
		return field_[i*size_x_ + (size_x_ + j%size_x_)]; 
	} else if (i >= 0 && i < size_y_) {
		return field_[i*size_x_ + j%size_x_]; 
	} else {
		return zero_;
	};
}