#include "./vector3_field.hpp"

// usefull functions:

int reflecting_begin_x_E(const vector3_field& _F, Axis field_component)
{
	switch (field_component) {
		case X: return 0;
		case Y: return 1;
		case Z: return 0;
	}
	return 0;
}

int reflecting_begin_y_E(const vector3_field& _F, Axis field_component)
{
	switch (field_component) {
		case X: return 1;
		case Y: return 0;
		case Z: return 0;
	}
	return 0;
}

int reflecting_end_x_E(const vector3_field& _F, Axis field_component)
{
	switch (field_component) {
		case X: return _F.size_x();
		case Y: return _F.size_x();
		case Z: return _F.size_x()-1;
	}
	return 0;
}

int reflecting_end_y_E(const vector3_field& _F, Axis field_component)
{
	switch (field_component) {
		case X: return _F.size_y();
		case Y: return _F.size_y();
		case Z: return _F.size_y()-1;
	}
	return 0;
}


int reflecting_end_x_B(const vector3_field& _F, Axis field_component)
{
	switch (field_component) {
		case X: return _F.size_y()-1;
		case Y: return _F.size_y();
		case Z: return _F.size_y();
	}
	return 0;
}

int reflecting_end_y_B(const vector3_field& _F, Axis field_component)
{
	switch (field_component) {
		case X: return _F.size_y();
		case Y: return _F.size_y()-1;
		case Z: return _F.size_y();
	}
	return 0;
}


int periodic_end_X(const vector3_field& _F, Axis field_component)
{
	switch (field_component) {
		case X: return _F.size_x();
		case Y: return _F.size_x();
		case Z: return _F.size_x();
	}
	return 0;
}

int periodic_end_Y(const vector3_field& _F, Axis field_component)
{
	switch (field_component) {
		case X: return _F.size_y();
		case Y: return _F.size_y();
		case Z: return _F.size_y();
	}
	return 0;
}


//#################################################################################################


int periodic_vector3_field::begin_x(Axis field_component) const
{ return 0; }

int periodic_vector3_field::begin_y(Axis field_component) const
{ return 0; }

int periodic_vector3_field::end_x(Axis field_component) const
{ return periodic_end_X(*this, field_component); }

int periodic_vector3_field::end_y(Axis field_component) const
{ return periodic_end_Y(*this, field_component); }

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


//#################################################################################################


vector3& reflecting_vector3_field::field_at(const int i, const int j)
{
	if ((i >= 0 && i < size_y_) && (j >= 0 && j < size_x_)){
		return field_[i*size_x_ + j]; 
	} else {
		zero_.x = 0;
		zero_.y = 0;
		zero_.z = 0;
		return zero_;
	};
}

vector3 reflecting_vector3_field::field_at(const int i, const int j) const
{
	if ((i >= 0 && i < size_y_) && (j >= 0 && j < size_x_)){
		return field_[i*size_x_ + j]; 
	} else {
		return zero_;
	};
}

//________________________________________________________________________

int reflecting_Electric_field::begin_x(Axis field_component) const 
{ return reflecting_begin_x_E(*this, field_component); }

int reflecting_Electric_field::begin_y(Axis field_component) const
{ return reflecting_begin_y_E(*this, field_component); }

int reflecting_Electric_field::end_x(Axis field_component) const
{ return reflecting_end_x_E(*this, field_component); }

int reflecting_Electric_field::end_y(Axis field_component) const
{ return reflecting_end_y_E(*this, field_component); }

//________________________________________________________________________

int reflecting_Magnetic_field::begin_x(Axis field_component) const 
{ return 0; }

int reflecting_Magnetic_field::begin_y(Axis field_component) const
{ return 0; }

int reflecting_Magnetic_field::end_x(Axis field_component) const
{ return reflecting_end_x_B(*this, field_component); }

int reflecting_Magnetic_field::end_y(Axis field_component) const
{ return reflecting_end_y_B(*this, field_component); }


//#################################################################################################