#include "point.hpp"

void reflective_Xboundary(Point& point, double SIZE_X)
{
	if ( point.r().x < 0.04 ) {
		point.r().x = 0.04;
		point.p().x = - point.p().x; 
	} else if ( point.r().x > SIZE_X - 0.04 ) {
		point.r().x = SIZE_X - 0.04;
		point.p().x = - point.p().x;
	}
}

void reflective_Yboundary(Point& point, double SIZE_Y)
{
	if ( point.r().y < 0.04 ) {
		point.r().y = 0.04;
		point.p().y *= -1; 
	} else if ( point.r().y > SIZE_Y - 0.04 ) {
		point.r().y = SIZE_Y - 0.04;
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