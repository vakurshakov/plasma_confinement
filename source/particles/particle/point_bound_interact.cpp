#include "point.hpp"
#include "../../constants.h"


void reflective_Xboundary(Point& point, double size_x)
{
	if ( point.x() < 0.5*dx ) {
		point.x() = 0.5*dx;
		point.px() *= -1; 
	}
	else if ( point.x() > size_x - 0.5*dx ) {
		point.x() = size_x - 0.5*dx;
		point.px() *= -1;
	}
}

void reflective_Yboundary(Point& point, double size_y)
{
	if ( point.y() < 0.5*dx ) {
		point.y() = 0.5*dx;
		point.py() *= -1; 
	}
	else if ( point.y() > size_y - 0.5*dx ) {
		point.y() = size_y - 0.5*dx;
		point.py() *= -1; 
	}
}

void periodic_Xboundary(Point& point, double size_x)
{
	if ( point.x() <= 0 ) {
		point.x() += size_x;
	}
	else if ( point.x() >= size_x ) {
		point.x() -= size_x;
	}
}

void periodic_Yboundary(Point& point, double size_y)
{
	if ( point.y() <= 0 ) {
		point.y() += size_y;
	}
	else if ( point.y() >= size_y ) {
		point.y() -= size_y;
	}
}