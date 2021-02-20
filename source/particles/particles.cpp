#include "./particles.hpp"

#include <iostream>

double frand() { return ((double)rand()/RAND_MAX); };

void coordinate_loader(std::string XY_distrib, int i, double& x, double& y)
{
	if (XY_distrib == "random") {
		x = frand()*SIZE_X*dx;  
		y = frand()*SIZE_Y*dy;  
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
};

class_particles::class_particles(double q, double m, double n, double Np,
		double (*form_factor)(double, double), int charge_cloud,
		std::string XY_distrib, vector2 p0)
		: m_q(q), m_m(m), m_n(n), m_Np(Np), m_form_factor(form_factor), m_charge_cloud(charge_cloud) {

	srand(time(NULL));
	int err = 0;
	for (int i = 0; i < (int)Np*SIZE_X*SIZE_Y + err; ++i) {

		double x, y;
		coordinate_loader(XY_distrib, i, x, y);
		
		double px = p0.x() + sin(2.*M_PI*frand())*sqrt(-2.*(Tx*me/mec2)*log(frand())); 
		double py = p0.y() + sin(2.*M_PI*frand())*sqrt(-2.*(Ty*me/mec2)*log(frand()));

		if (std::isinf(px) | std::isinf(py)) { 
			++err;
			continue;
		}
		
		vector2 r(x, y);
		vector3 p(px, py, 0);
		
		particle temp_(r, p);
		m_particles.push_back(temp_);
	}
}

class_particles::class_particles(double q, double m, double n, double Np,
		double (*form_factor)(double, double), int charge_cloud,
		std::string XY_distrib, vector3 p0)
		: m_q(q), m_m(m), m_n(n), m_Np(Np), m_form_factor(form_factor), m_charge_cloud(charge_cloud) {

	srand(time(NULL));
	int err = 0;
	for (int i = 0; i < (int)Np*SIZE_X*SIZE_Y + err; ++i) {

		double x, y;
		coordinate_loader(XY_distrib, i, x, y);
		
		double px = p0.x() + sin(2.*M_PI*frand())*sqrt(-2.*(Tx*me/mec2)*log(frand())); 
		double py = p0.y() + sin(2.*M_PI*frand())*sqrt(-2.*(Ty*me/mec2)*log(frand()));
		double pz = p0.z() + sin(2.*M_PI*frand())*sqrt(-2.*(Tz*me/mec2)*log(frand()));

		if (std::isinf(px) | std::isinf(py) | std::isinf(pz)) { 
			++err;
			continue;
		}
		
		vector2 r(x, y);
		vector3 p(px, py, pz);
		
		particle temp_(r, p);
		m_particles.push_back(temp_);
	}
}

class_particles::class_particles(double q, double m, double n, double Np,
		double (*form_factor)(double, double), int charge_cloud,
		std::string XY_distrib, std::string P_distrib)
		: m_q(q), m_m(m), m_n(n), m_Np(Np), m_form_factor(form_factor), m_charge_cloud(charge_cloud) {

	srand(time(NULL));
	int err = 0;
	for (int i = 0; i < (int)Np*SIZE_X*SIZE_Y + err; ++i) {

		double x, y;
		coordinate_loader(XY_distrib, i, x, y);
		
		double px, py;

		if (P_distrib == std::string("X_Oscillations")) {
			double px = 0.01*cos(2*M_PI*x/(SIZE_X*dx)) + sin(2.*M_PI*frand())*sqrt(-2.*(Tx*me/mec2)*log(frand())); 
			double py = 								 sin(2.*M_PI*frand())*sqrt(-2.*(Ty*me/mec2)*log(frand()));
		}
		else if (P_distrib == std::string("XY_Oscillations")) {
			double px = 0.01*cos(2*M_PI*x/(SIZE_X*dx)) + sin(2.*M_PI*frand())*sqrt(-2.*(Tx*me/mec2)*log(frand())); 
			double py = 0.01*cos(2*M_PI*y/(SIZE_Y*dy)) + sin(2.*M_PI*frand())*sqrt(-2.*(Ty*me/mec2)*log(frand()));
		}

		if (std::isinf(px) | std::isinf(py)) { 
			++err;
			continue;
		}
		
		vector2 r(x, y);
		vector3 p(px, py, 0);		
		
		particle temp_(r, p);
		m_particles.push_back(temp_);		
	}
}