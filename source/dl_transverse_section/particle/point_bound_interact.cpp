#include "./point.hpp"

void reflective_Xboundary(Point& particle, double SIZE_X)
{
	if ( particle.r().x < 0.04 ) {
		particle.r().x = 0.04;
		particle.p().x = - particle.p().x; 
	} else if ( particle.r().x > SIZE_X - 0.04 ) {
		particle.r().x = SIZE_X - 0.04;
		particle.p().x = - particle.p().x;
	}
}

void reflective_Yboundary(Point& particle, double SIZE_Y)
{
	if ( particle.r().y < 0.04 ) {
		particle.r().y = 0.04;
		particle.p().y *= -1; 
	} else if ( particle.r().y > SIZE_Y - 0.04 ) {
		particle.r().y = SIZE_Y - 0.04;
		particle.p().y *= -1; 
	}
}

void periodic_Xboundary(Point& particle, double SIZE_X)
{
	if 		( particle.r().x <= 0 	 ) { particle.r().x += SIZE_X; }
	else if ( particle.r().x >= SIZE_X ) { particle.r().x -= SIZE_X; }
}

void periodic_Yboundary(Point& particle, double SIZE_Y)
{
	if 		( particle.r().y <= 0 	 ) { particle.r().y += SIZE_Y; }
	else if ( particle.r().y >= SIZE_Y ) { particle.r().y -= SIZE_Y; }
}