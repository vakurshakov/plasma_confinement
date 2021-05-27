#ifndef MIRROR_H 
#define MIRROR_H

//#################################################################################################

#include "../../vectors/vector_classes.hpp"
#include "../../constants.h"
#include <cmath>

using namespace std;

class Magnetic_mirror {
public:
	Magnetic_mirror(double _B_m, double _x_m, double _B_l, double _x_l)
	: B_m(_B_m), x_m(_x_m), B_l(_B_l), x_l(_x_l) {
		
		Ry = sqrt( B_l/(B_m - B_l) )*fabs( x_l - x_m );
		I  = double(M_PI)*( B_m*Ry );
	};

	vector3 return_field(vector2 r) const {
		// функция, возвращающая магнитное поле в точке r от бесконечно тонкой
		// линии тока; параметры линнии тока описываются структурой Magnetic_mirror 

		vector3 temp;

		// центрирование на пробку по X
		double x = r.x - x_m;
		// центрирование на середину счётной области по Y
		double y = r.y - 0.5*SIZE_Y*dy;
		

		double Dpy = Ry + y; 
		double Dmy = Ry - y;
	
		temp.x = I/double(M_PI)*(Ry*( Ry*Ry - y*y + x*x ))/
				 ( (x*x + Dpy*Dpy) * (x*x + Dmy*Dmy) );	
		
		temp.y = I/double(M_PI)*(2.*x*y*Ry)/
				 ( (x*x + Dpy*Dpy) * (x*x + Dmy*Dmy) ); 

		temp.z = 0;
	
		return temp;
	}

protected:
	// инициализируемые величины:
	double B_m; // поле в пробке
	double x_m; // положение пробки на оси X в c/wp
	double B_l; // поле на НЕКОТОРОМ расстоянии от пробки
	double x_l; // расстояние от пробки, где задано B_l (в c/wp)

private:
	// рассчитываемые величины:
	double Ry; // 0.5(расстояния между витками с током) (радиус пробки)
	double I;  // величина токов в безразмерных единицах
};

const Magnetic_mirror mirror1(Bz0-Bzl, x_m1, Bzl, x_m2);
const Magnetic_mirror mirror2(Bz0-Bzl, x_m2, Bzl, x_m1);

//#################################################################################################

#endif