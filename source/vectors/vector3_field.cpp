#include "vector3_field.hpp"

// usefull functions:

int rx_begin_E(const vector3_field& _F, Axis field_component)
{
	switch (field_component) {
		case X: return 0;
		case Y: return 1;
		case Z: return 0;
	}
	return 0;
}

int ry_begin_E(const vector3_field& _F, Axis field_component)
{
	switch (field_component) {
		case X: return 1;
		case Y: return 0;
		case Z: return 0;
	}
	return 0;
}

int rx_end_E(const vector3_field& _F, Axis field_component)
{
	switch (field_component) {
		case X: return _F.size_x();
		case Y: return _F.size_x();
		case Z: return _F.size_x()-1;
	}
	return 0;
}

int ry_end_E(const vector3_field& _F, Axis field_component)
{
	switch (field_component) {
		case X: return _F.size_y();
		case Y: return _F.size_y();
		case Z: return _F.size_y()-1;
	}
	return 0;
}


int rx_end_B(const vector3_field& _F, Axis field_component)
{
	switch (field_component) {
		case X: return _F.size_x()-1;
		case Y: return _F.size_x();
		case Z: return _F.size_x();
	}
	return 0;
}

int ry_end_B(const vector3_field& _F, Axis field_component)
{
	switch (field_component) {
		case X: return _F.size_y();
		case Y: return _F.size_y()-1;
		case Z: return _F.size_y();
	}
	return 0;
}


int px_end(const vector3_field& _F)
{
	return _F.size_x();
}

int py_end(const vector3_field& _F)
{
	return _F.size_y();
}


//#################################################################################################


	int px_py_vector3_field::begin_x(Axis field_component) const
	{ return 0; }
	
	int px_py_vector3_field::begin_y(Axis field_component) const
	{ return 0; }
	
	int px_py_vector3_field::end_x(Axis field_component) const
	{ return px_end(*this); }
	
	int px_py_vector3_field::end_y(Axis field_component) const
	{ return py_end(*this); }
	
	vector3& px_py_vector3_field::field_at(int i, int j)
	{
		if (i < 0 && j < 0) {
			return field_[(size_y_ + i%size_y_)*size_x_ + (size_x_ + j%size_x_)]; 
		} else if (i < 0) {
			return field_[(size_y_ + i%size_y_)*size_x_ + j%size_x_]; 
		} else if (j < 0) {
			return field_[(i%size_y_)*size_x_ + (size_x_ + j%size_x_)]; 
		} else return field_[(i%size_y_)*size_x_ + j%size_x_];
	}
	
	vector3 px_py_vector3_field::field_at(int i, int j) const
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


	vector3& rx_ry_vector3_field::field_at(int i, int j)
	{
		if ((i >= 0 && i < size_y_) && (j >= 0 && j < size_x_)){
			return field_[i*size_x_ + j]; 
		} else {
			zero_.x = 0;
			zero_.y = 0;
			zero_.z = 0;
			return zero_;
		}
	}
	
	vector3 rx_ry_vector3_field::field_at(int i, int j) const
	{
		if ((i >= 0 && i < size_y_) && (j >= 0 && j < size_x_)){
			return field_[i*size_x_ + j]; 
		} else {
			return zero_;
		}
	}

	//________________________________________________________________________
	
		int rx_ry_Electric_field::begin_x(Axis field_component) const 
		{ return rx_begin_E(*this, field_component); }
		
		int rx_ry_Electric_field::begin_y(Axis field_component) const
		{ return ry_begin_E(*this, field_component); }
		
		int rx_ry_Electric_field::end_x(Axis field_component) const
		{ return rx_end_E(*this, field_component); }
		
		int rx_ry_Electric_field::end_y(Axis field_component) const
		{ return ry_end_E(*this, field_component); }
	
	//________________________________________________________________________
	
		int rx_ry_Magnetic_field::begin_x(Axis field_component) const 
		{ return 0; }
		
		int rx_ry_Magnetic_field::begin_y(Axis field_component) const
		{ return 0; }
		
		int rx_ry_Magnetic_field::end_x(Axis field_component) const
		{ return rx_end_B(*this, field_component); }
		
		int rx_ry_Magnetic_field::end_y(Axis field_component) const
		{ return ry_end_B(*this, field_component); }


//#################################################################################################
	
	vector3& px_ry_vector3_field::field_at(int i, int j)
	{
		if (i >= 0 && i < size_y_){
			if ( j < 0 ) { 
				return field_[i*size_x_ + (size_x_ + j%size_x_)];
			} else {
				return field_[i*size_x_ + j%size_x_];
			}
		} else {
			zero_.x = 0;
			zero_.y = 0;
			zero_.z = 0;
			return zero_;
		}
	}
	
	vector3 px_ry_vector3_field::field_at(int i, int j) const
	{
		if (i >= 0 && i < size_y_){
			if ( j < 0 ) { 
				return field_[i*size_x_ + (size_x_ + j%size_x_)];
			} else {
				return field_[i*size_x_ + j%size_x_];
			}
		} else {
			return zero_;
		}
	}
	
	//________________________________________________________________________
	
		int px_ry_Electric_field::begin_x(Axis field_component) const 
		{ return 0; }
		
		int px_ry_Electric_field::begin_y(Axis field_component) const
		{ return ry_begin_E(*this, field_component); }
		
		int px_ry_Electric_field::end_x(Axis field_component) const
		{ return px_end(*this); }
		
		int px_ry_Electric_field::end_y(Axis field_component) const
		{ return ry_end_E(*this, field_component); }
	
	//________________________________________________________________________
	
		int px_ry_Magnetic_field::begin_x(Axis field_component) const 
		{ return 0; }
		
		int px_ry_Magnetic_field::begin_y(Axis field_component) const
		{ return 0; }
		
		int px_ry_Magnetic_field::end_x(Axis field_component) const
		{ return px_end(*this); }
		
		int px_ry_Magnetic_field::end_y(Axis field_component) const
		{ return ry_end_B(*this, field_component); }


//#################################################################################################
	
	vector3& rx_py_vector3_field::field_at(int i, int j)
	{
		if (j >= 0 && j < size_x_){
			if ( i < 0 ) { 
				return field_[(size_y_ + i%size_y_)*size_x_ + j];
			} else {
				return field_[(i%size_y_)*size_x_ + j];
			}
		} else {
			zero_.x = 0;
			zero_.y = 0;
			zero_.z = 0;
			return zero_;
		}
	}
	
	vector3 rx_py_vector3_field::field_at(int i, int j) const
	{
			if (j >= 0 && j < size_x_){
			if ( i < 0 ) { 
				return field_[(size_y_ + i%size_y_)*size_x_ + j];
			} else {
				return field_[(i%size_y_)*size_x_ + j];
			}
		} else {
			return zero_;
		}
	}
	
	//________________________________________________________________________
	
		int rx_py_Electric_field::begin_x(Axis field_component) const 
		{ return rx_begin_E(*this, field_component); }
		
		int rx_py_Electric_field::begin_y(Axis field_component) const
		{ return 0; }
		
		int rx_py_Electric_field::end_x(Axis field_component) const
		{ return rx_end_E(*this, field_component); }
		
		int rx_py_Electric_field::end_y(Axis field_component) const
		{ return py_end(*this); }
	
	//________________________________________________________________________
	
		int rx_py_Magnetic_field::begin_x(Axis field_component) const 
		{ return 0; }
		
		int rx_py_Magnetic_field::begin_y(Axis field_component) const
		{ return 0; }
		
		int rx_py_Magnetic_field::end_x(Axis field_component) const
		{ return rx_end_B(*this, field_component); }
		
		int rx_py_Magnetic_field::end_y(Axis field_component) const
		{ return py_end(*this); }


//#################################################################################################
