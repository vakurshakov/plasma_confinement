#include "point.hpp"
#include "../../constants.h"

void reflective_Xboundary(Point& point, double SIZE_X)
{
	if ( point.r().x < dx ) {
		point.r().x = dx;
		point.p().x *= -1; 
	} else if ( point.r().x > SIZE_X - dx ) {
		point.r().x = SIZE_X - dx;
		point.p().x *= -1;
	}
}

void reflective_Yboundary(Point& point, double SIZE_Y)
{
	if ( point.r().y < dx ) {
		point.r().y = dx;
		point.p().y *= -1; 
	} else if ( point.r().y > SIZE_Y - dx ) {
		point.r().y = SIZE_Y - dx;
		point.p().y *= -1; 
	}
}

void periodic_Xboundary(Point& point, double SIZE_X)
{
	if 		( point.r().x <= 0 	 ) { point.r().x += SIZE_X; }
	else if ( point.r().x >= SIZE_X ) { point.r().x -= SIZE_X; }
}

void periodic_Yboundary(Point& point, double SIZE_Y)
{
	if 		( point.r().y <= 0 	 ) { point.r().y += SIZE_Y; }
	else if ( point.r().y >= SIZE_Y ) { point.r().y -= SIZE_Y; }
}