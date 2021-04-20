#include "../particles/species_description.hpp"
#include <iostream>

double frand() { return ((double)rand()/RAND_MAX); }

void coordinate_loader(string& XY_distrib, int i,
	double cX, double cY, double Xm, double Ym, double& x, double& y)
{
	if (XY_distrib == "random") {
		x = cX + (2*Xm*frand() - Xm);  
		y = cY + (2*Ym*frand() - Ym);  
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
		x = ((i/int(Np)) % SIZE_X)*dx + ((i%int(Np)) % divider)*dx/divider;
		y = ((i/int(Np)) / SIZE_X)*dy + ((i%int(Np)) / divider)*dy/(int(Np)/divider); 
	}
	else if (XY_distrib == "circle_random") {
		do
		{
			x = cX + (2*Xm*frand() - Xm);  
			y = cY + (2*Ym*frand() - Ym);	
		} 	while ((x - cX)*(x - cX) + (y - cY)*(y - cY) > Xm*Ym);
	}
}


void load_p02d_particles(Species_description& sort, double Np,
		string XY_distrib, double cX, double cY, double Xm, double Ym, const vector2& p0)
{
	sort.Np_ = Np;

	srand(time(NULL));
	int err = 0;
	
	for (int i = 0; i < int(Np*(2*Xm/dx)*(2*Ym/dy)) + err; ++i) {

		double x, y;
		coordinate_loader(XY_distrib, i, cX, cY, Xm, Ym, x, y);
		
		double px = p0.x + sin(2.*M_PI*frand())*sqrt(-2.*(Tx*sort.m_/mec2)*log(frand())); 
		double py = p0.y + sin(2.*M_PI*frand())*sqrt(-2.*(Ty*sort.m_/mec2)*log(frand()));

		if (isinf(px) | isinf(py)) { 
			++err;
			continue;
		}
		
		vector2 r(x, y);
		vector3 p(px, py, 0);
		
		Particle temp_(r, p);
		sort.particles_.push_back(temp_);
	}
}

void load_p03d_particles(Species_description& sort, double Np,
		string XY_distrib, double cX, double cY, double Xm, double Ym, const vector3& p0)
{
	sort.Np_ = Np;

	srand(time(NULL));
	int err = 0;
	for (int i = 0; i < int(Np*(2*Xm/dx)*(2*Ym/dy)) + err; ++i) {

		double x, y;
		coordinate_loader(XY_distrib, i, cX, cY, Xm, Ym, x, y);
		
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
}

void load_chosen_distribution(Species_description& sort, double Np,
		string XY_distrib, double cX, double cY, double Xm, double Ym, string P_distrib)
{
	sort.Np_ = Np;
	
	srand(time(NULL));
	int err = 0;
	for (int i = 0; i < int(Np*(2*Xm/dx)*(2*Ym/dy)) + err; ++i) {

		double x, y;
		coordinate_loader(XY_distrib, i, cX, cY, Xm, Ym, x, y);
		
		double px, py;

		if (P_distrib == string("X_Oscillations")) {
			double px = 0.01*cos(2*M_PI*x/(SIZE_X*dx)) + sin(2.*M_PI*frand())*sqrt(-2.*(Tx*sort.m_/mec2)*log(frand())); 
			double py = 								 sin(2.*M_PI*frand())*sqrt(-2.*(Ty*sort.m_/mec2)*log(frand()));
		}
		else if (P_distrib == string("XY_Oscillations")) {
			double px = 0.01*cos(2*M_PI*x/(SIZE_X*dx)) + sin(2.*M_PI*frand())*sqrt(-2.*(Tx*sort.m_/mec2)*log(frand())); 
			double py = 0.01*cos(2*M_PI*y/(SIZE_Y*dy)) + sin(2.*M_PI*frand())*sqrt(-2.*(Ty*sort.m_/mec2)*log(frand()));
		}

		if (isinf(px) | isinf(py)) { 
			++err;
			continue;
		}
		
		vector2 r(x, y);
		vector3 p(px, py, 0);		
		
		Particle temp_(r, p);
		sort.particles_.push_back(temp_);		
	}
}