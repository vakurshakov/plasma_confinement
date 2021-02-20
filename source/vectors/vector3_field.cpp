#include "vector3_field.hpp"

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
	if ((i >= 1 && i < msize_y-1) && (j >= 1 && j < msize_x-1)){
		return field[i*msize_x + j]; 
	} else {
		zero.x() = 0;
		zero.y() = 0;
		zero.z() = 0;
		return zero;
	};
}

vector3 reflective_vector3_field::field_at(const int i, const int j) const
{
	if ((i >= 1 && i < msize_y-1) && (j >= 1 && j < msize_x-1)){
		return field[i*msize_x + j]; 
	} else {
		return zero;
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
		return field[(msize_y + i%msize_y)*msize_x + (msize_x + j%msize_x)]; 
	} else if (i < 0) {
		return field[(msize_y + i%msize_y)*msize_x + j%msize_x]; 
	} else if (j < 0) {
		return field[(i%msize_y)*msize_x + (msize_x + j%msize_x)]; 
	} else return field[(i%msize_y)*msize_x + j%msize_x];
}

vector3 periodic_vector3_field::field_at(const int i, const int j) const
{
	if (i < 0 && j < 0) {
		return field[(msize_y + i%msize_y)*msize_x + (msize_x + j%msize_x)]; 
	} else if (i < 0) {
		return field[(msize_y + i%msize_y)*msize_x + j%msize_x]; 
	} else if (j < 0) {
		return field[(i%msize_y)*msize_x + (msize_x + j%msize_x)]; 
	} else return field[(i%msize_y)*msize_x + j%msize_x];
}

//-------------------------------------------------------------------------------------------------
// reflective horisontaly & periodic verticaly

int rh_pv_vector3_field::end_x(Axis field_component) const
{ return reflecting_x(*this, field_component); }

int rh_pv_vector3_field::end_y(Axis field_component) const
{ return periodic_Y(*this, field_component); }

vector3& rh_pv_vector3_field::field_at(const int i, const int j)
{
	if (i < 0 && (j >= 0 && j < msize_x)) {
		return field[(msize_y + i%msize_y)*msize_x + j]; 
	} else if (j >= 0 && j < msize_x) {
		return field[(i%msize_y)*msize_x + j]; 
	} else {
		zero.x() = 0;
		zero.y() = 0;
		zero.z() = 0;
		return zero;
	};
}

vector3 rh_pv_vector3_field::field_at(const int i, const int j) const
{
	if (i < 0 && (j >= 0 && j < msize_x)) {
		return field[(msize_y + i%msize_y)*msize_x + j]; 
	} else if (j >= 0 && j < msize_x) {
		return field[(i%msize_y)*msize_x + j]; 
	} else {
		return zero;
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
	if ((i >= 0 && i < msize_y) && j < 0) {
		return field[i*msize_x + (msize_x + j%msize_x)]; 
	} else if (i >= 0 && i < msize_y) {
		return field[i*msize_x + j%msize_x]; 
	} else {
		zero.x() = 0;
		zero.y() = 0;
		zero.z() = 0;
		return zero;
	};
}

vector3 rv_ph_vector3_field::field_at(const int i, const int j) const
{
	if ((i >= 0 && i < msize_y) && j < 0) {
		return field[i*msize_x + (msize_x + j%msize_x)]; 
	} else if (i >= 0 && i < msize_y) {
		return field[i*msize_x + j%msize_x]; 
	} else {
		return zero;
	};
}