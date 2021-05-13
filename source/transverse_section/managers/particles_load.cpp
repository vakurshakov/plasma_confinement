#include "../particles/species_description.hpp"
#include <iostream>

double frand() { return ((double)rand()/RAND_MAX); }

void load_coordinates(string& XY_distrib, int i, int Np,
	double cx, double cy, double xm, double ym, double& x, double& y)
{
	if (XY_distrib == "random") {
		x = cx + (2*xm*frand() - xm);  
		y = cy + (2*ym*frand() - ym);  
	}
	else if (XY_distrib == "periodic") {
		// i%Np -- номер от нуля до Np-1 частицы в одной ячейке
		// i/Np -- список индексов, эквивалентный перебору ячеек
		// индекс ячейки по Ox: (i/Np % SIZE_X), Oy: (i/Np / SIZE_X)

		// сколько по длинне в ячейку влазит? Это надо искать делители Np
		// при том лучше, конечно, от половины Np начинать в обе стороны искать

		// NOTE: пока эта проблема решена так: divider -- число, показывающее 
		//		 сколько частиц будет уложено в одну ячейку вдоль оси Ox; 
		
		int divider = 2;
		x = (cx - xm) + ((i%Np) % divider)*dx/divider;
		y = (cy - ym) + ((i%Np) / divider)*dy/(Np/divider); 
	}
}

void load_p02d_particles(Species_description& sort,
		string XY_distrib, double p0)
{
	if ( XY_distrib.find("ring") == 0 ) {

		int divider = XY_distrib.find('_');
		int end = XY_distrib.size();

		string distrib = XY_distrib.substr(divider+1,end);	

		srand(time(NULL));

		for (int nx = 0; nx < SIZE_X; ++nx) {
		for (int ny = 0; ny < SIZE_Y; ++ny) {
			
			double cx = (nx+0.5 - 0.5*SIZE_X)*dx;
			double cy = (ny+0.5 - 0.5*SIZE_Y)*dy;
			
			if ( (r_larm-dr <= sqrt(cx*cx + cy*cy)) and (sqrt(cx*cx + cy*cy) <= r_larm+dr) ) {
			
				int err = 0;			
				for (int i = 0; i < sort.Np_ + err; ++i) {
				
					double x, y;
					
					load_coordinates(distrib, i, sort.Np_, cx+0.5*SIZE_X*dx, cy+0.5*SIZE_Y*dy, 0.5*dx, 0.5*dy, x, y);

					double r = sqrt((x-0.5*SIZE_X*dx)*(x-0.5*SIZE_X*dx) + (y-0.5*SIZE_Y*dy)*(y-0.5*SIZE_Y*dy));

					double px = p0*( +(y-0.5*SIZE_Y*dy)/r ) + sin(2.*M_PI*frand())*sqrt(-2.*(Tx*sort.m_/mec2)*log(frand())); 
					double py = p0*( -(x-0.5*SIZE_X*dx)/r ) + sin(2.*M_PI*frand())*sqrt(-2.*(Ty*sort.m_/mec2)*log(frand()));
						
					if (isinf(px) | isinf(py)) { 
						++err;
						continue;
					}
								
					vector2 _r(x, y);
					vector3 _p(px, py, 0);
								
					Particle temp_(_r, _p);
					sort.particles_.push_back(temp_);
				}
			}
		}}

	} else if ( XY_distrib.find("circle") == 0 ) {

		int divider = XY_distrib.find('_');
		int end = XY_distrib.size();

		string distrib = XY_distrib.substr(divider+1,end);	

		srand(time(NULL));

		for (int nx = 0; nx < SIZE_X; ++nx) {
		for (int ny = 0; ny < SIZE_Y; ++ny) {
			
			double cx = (nx+0.5 - 0.5*SIZE_X)*dx;
			double cy = (ny+0.5 - 0.5*SIZE_Y)*dy;
			
			if ( sqrt(cx*cx + cy*cy) <= (r_larm+dr)*r_prop ) {
				
				int err = 0;			
				for (int i = 0; i < sort.Np_ + err; ++i) {
				
					double x, y;
					
					load_coordinates(distrib, i, sort.Np_, cx+0.5*SIZE_X*dx, cy+0.5*SIZE_Y*dy, 0.5*dx, 0.5*dy, x, y);
			
					double px = sin(2.*M_PI*frand())*sqrt(-2.*(Tx*sort.m_/mec2)*log(frand())); 
					double py = sin(2.*M_PI*frand())*sqrt(-2.*(Ty*sort.m_/mec2)*log(frand()));
						
					if (isinf(px) | isinf(py)) { 
						++err;
						continue;
					}
								
					vector2 _r(x, y);
					vector3 _p(px, py, 0);
								
					Particle temp_(_r, _p);
					sort.particles_.push_back(temp_);
				}
			}
		}}
	}
}

void load_p03d_particles(Species_description& sort,
		string XY_distrib, double p0)
{
/*
	srand(time(NULL));
	int err = 0;
	for (int i = 0; i < int(Np*(2*Xm/dx)*(2*Ym/dy)) + err; ++i) {

		double x, y;
		load_coordinates(XY_distrib, i, cX, cY, Xm, Ym, x, y);
		
		double px = p0.x + sin(2.*M_PI*frand())*sqrt(-2.*(Tx*sort.m_/mec2)*log(frand())); 
		double py = p0.y + sin(2.*M_PI*frand())*sqrt(-2.*(Ty*sort.m_/mec2)*log(frand()));
		double pz = p0.z + sin(2.*M_PI*frand())*sqrt(-2.*(Tz*sort.m_/mec2)*log(frand()));

		if (isinf(px) | isinf(py) | isinf(pz)) { 
			++err;
			continue;
		}
		
		vector2 r(x, y);
		vector3 p(px, py, pz);
		
		Particle temp_(r, p);
		sort.particles_.push_back(temp_);
	}
*/
}