#include "vector3_field.hpp"

/*
*	Возникла ошибка неправильного обращения к массивам (для записи). Исправление
*	могло бы быть организовано просто заменой всех границ в get_vector на 
*	ix(y)_first(last), однако это решение очень медлительное из-за частых вызовов.
*	Оптимизировать его было решено следующим образом:
*	   1. Использовать для константных методов полные границы, заданные константами,  
*	   2. Для неконстантных методов использовать простейший контейнер с границами.
*/

//_______ vector field ____________________________________________________________________________

double&  vector3_field::x(int ny, int nx) { return get_vector(X, ny, nx).x(); }
double&  vector3_field::y(int ny, int nx) { return get_vector(Y, ny, nx).y(); }
double&  vector3_field::z(int ny, int nx) { return get_vector(Z, ny, nx).z(); }

const double&  vector3_field::x(int ny, int nx) const { return get_vector(ny, nx).x(); }
const double&  vector3_field::y(int ny, int nx) const { return get_vector(ny, nx).y(); }
const double&  vector3_field::z(int ny, int nx) const { return get_vector(ny, nx).z(); }


//_______ periodic vector field ___________________________________________________________________

int px_py_vector3_field::ix_first (Axis) const { return 0; }
int px_py_vector3_field::iy_first (Axis) const { return 0; }
int px_py_vector3_field::ix_last  (Axis) const { return size_x_; }
int px_py_vector3_field::iy_last  (Axis) const { return size_y_; }

const vector3& px_py_vector3_field::get_vector(int ny, int nx) const {
	if (nx < 0 && ny < 0) {
		return field_[(size_y_ + ny%size_y_)*size_x_ + (size_x_ + nx%size_x_)]; 
	}
	else if (nx < 0) {
		return field_[(ny%size_y_)*size_x_ + (size_x_ + nx%size_x_)]; 
	}
	else if (ny < 0) {
		return field_[(size_y_ + ny%size_y_)*size_x_ + nx%size_x_]; 
	}
	else return field_[(ny%size_y_)*size_x_ + nx%size_x_];
}

vector3& px_py_vector3_field::get_vector(Axis axis, int ny, int nx) {
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


//_______ reflecting vector field _________________________________________________________________

rx_ry_vector3_field::rx_ry_vector3_field(std::string type, int size_x, int size_y)
	:	vector3_field(size_x, size_y)
{
	if ( type == "Electric" ) {
		bound[left  + X] = 0; 		 bound[left  + Y] = 1;		  bound[left  + Z] = 0;
		bound[right + X] = size_x_;  bound[right + Y] = size_x;	  bound[right + Z] = size_x_-1;
		bound[lower + X] = 1; 		 bound[lower + Y] = 0; 		  bound[lower + Z] = 0;
		bound[upper + X] = size_y_;  bound[upper + Y] = size_y_;  bound[upper + Z] = size_y_-1;
	}
	else if ( type == "Magnetic" ) {
		bound[left  + X] = 0;		   bound[left  + Y] = 0;		  bound[left  + Z] = 0;
		bound[right + X] = size_x_-1;  bound[right + Y] = size_x; 	  bound[right + Z] = size_x_;
		bound[lower + X] = 0;		   bound[lower + Y] = 0;		  bound[lower + Z] = 0;
		bound[upper + X] = size_y_;	   bound[upper + Y] = size_y_-1;  bound[upper + Z] = size_y_;
	}
} 
int rx_ry_vector3_field::ix_first (Axis axis) const { return bound[left  + axis]; }
int rx_ry_vector3_field::ix_last  (Axis axis) const { return bound[right + axis]; }
int rx_ry_vector3_field::iy_first (Axis axis) const { return bound[lower + axis]; }
int rx_ry_vector3_field::iy_last  (Axis axis) const { return bound[upper + axis]; }

const vector3& rx_ry_vector3_field::get_vector(int ny, int nx) const
{
	if ((nx >= 0 && nx < size_x_)
	 && (ny >= 0 && ny < size_y_)) {
		return field_[ny*size_x_ + nx]; 
	}
	else { return zero_; }
}

vector3& rx_ry_vector3_field::get_vector(Axis axis, int ny, int nx)
{
	if ((nx >= this->ix_first(axis) && nx < this->ix_last(axis))
	 && (ny >= this->iy_first(axis) && ny < this->iy_last(axis))) {
		return field_[ny*size_x_ + nx]; 
	}
	else {
		zero_ = {0, 0, 0};
		return zero_;
	}
}

//_______ periodic on x axis and reflecting on y axis _____________________________________________

px_ry_vector3_field::px_ry_vector3_field(std::string type, int size_x, int size_y)
	:	vector3_field(size_x, size_y)
{
	if ( type == "Electric" ) {
		bound[lower + X] = 1; 		 bound[lower + Y] = 0;		  bound[lower + Z] = 0;
		bound[upper + X] = size_y_;  bound[upper + Y] = size_y_;  bound[upper + Z] = size_y_-1;
	}
	else if ( type == "Magnetic" ) {
		bound[lower + X] = 0; 		 bound[lower + Y] = 0; 		 	bound[lower + Z] = 0;
		bound[upper + X] = size_y_;	 bound[upper + Y] = size_y_-1; 	bound[upper + Z] = size_y_;
	}
}

int px_ry_vector3_field::ix_first (Axis axis) const { return 0; }
int px_ry_vector3_field::iy_first (Axis axis) const { return bound[lower + axis]; }
int px_ry_vector3_field::ix_last  (Axis axis) const { return size_x_; }
int px_ry_vector3_field::iy_last  (Axis axis) const { return bound[upper + axis]; }

const vector3& px_ry_vector3_field::get_vector(int ny, int nx) const
{
	if (ny >= 0 && ny < size_y_) {
		if ( nx < 0 ) { 
			return field_[ny*size_x_ + (size_x_ + nx%size_x_)];
		} else {
			return field_[ny*size_x_ + nx%size_x_];
		}
	} else { return zero_; }
}

vector3& px_ry_vector3_field::get_vector(Axis axis, int ny, int nx)
{
	if (ny >= this->iy_first(axis) && ny < this->iy_last(axis)) {
		if ( nx < this->ix_first(axis) ) { 
			return field_[ny*size_x_ + (size_x_ + nx%size_x_)];
		} else {
			return field_[ny*size_x_ + nx%size_x_];
		}
	} else {
		zero_ = {0, 0, 0};
		return zero_;
	}
}


//_______ reflecting on x axis and periodic on y axis _____________________________________________

rx_py_vector3_field::rx_py_vector3_field(std::string type, int size_x, int size_y)
	:	vector3_field(size_x, size_y)
{
	if ( type == "Electric" ) {
		bound[left  + X] = 0; 		 bound[left  + Y] = 1; 		 bound[left  + Z] = 0;
		bound[right + X] = size_x_;  bound[right + Y] = size_x;  bound[right + Z] = size_x_-1;
			}
	else if ( type == "Magnetic" ) {
		bound[left  + X] = 0; 		   bound[left  + Y] = 0; 	   bound[left  + Z]	= 0;
		bound[right + X] = size_x_-1;  bound[right + Y] = size_x;  bound[right + Z] = size_x_;
	}
}

int rx_py_vector3_field::ix_first (Axis axis) const { return bound[left  + axis]; }
int rx_py_vector3_field::iy_first (Axis axis) const { return 0; }
int rx_py_vector3_field::ix_last  (Axis axis) const { return bound[right + axis]; }
int rx_py_vector3_field::iy_last  (Axis axis) const { return size_y_; }
	
const vector3& rx_py_vector3_field::get_vector(int ny, int nx) const
{
	if (nx >= 0 && nx < size_x_){
		if ( ny < 0 ) { 
			return field_[(size_y_ + ny%size_y_)*size_x_ + nx];
		} else {
			return field_[(ny%size_y_)*size_x_ + nx];
		}
	} else { return zero_; }
}

vector3& rx_py_vector3_field::get_vector(Axis axis, int ny, int nx)
{
	if (nx >= this->ix_first(axis) && nx < this->ix_last(axis)){
		if ( ny < this->iy_first(axis) ) { 
			return field_[(size_y_ + ny%size_y_)*size_x_ + nx];
		} else {
			return field_[(ny%size_y_)*size_x_ + nx];
		}
	} else {
		zero_ = {0, 0, 0};
		return zero_;
	}
}
