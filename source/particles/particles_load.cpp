#include "./particles_load.hpp"

#include <cmath>

#include "./particle/point.hpp"
#include "../vectors/vector_classes.hpp"
#include "../constants.h"


double frand() { return static_cast<double>(rand())/RAND_MAX; }

// cell filling -------------------------------------------------------------------------

void fill_randomly(int sequential_number, int Np,
	int cell_number_nx, int cell_number_ny,
	double* x, double* y)
{
		*x = (cell_number_nx + frand())*dx;  
		*y = (cell_number_ny + frand())*dy;  
}

void fill_periodically(int sequential_number, int Np,
	int cell_number_nx, int cell_number_ny,
	double* x, double* y)
{
	/* i%Np -- номер от нуля до Np-1 частицы в одной ячейке
	 * i/Np -- список индексов, эквивалентный перебору ячеек
	 * индекс ячейки по Ox: (i/Np % divider), Oy: (i/Np / divider).
	 *
	 * 	Cколько по длинне в ячейку влазит? Это надо искать делители Np
	 * при том лучше, конечно, от половины Np начинать в обе стороны искать
	 * Пока эта проблема решена так: divider -- число, показывающее 
	 * сколько частиц будет уложено в одну ячейку вдоль оси Ox; 
	 */
	
	int divider = 2;
	*x = cell_number_nx*dx + ((sequential_number%Np) % divider)*dx/divider;
	*y = cell_number_ny*dy + ((sequential_number%Np) / divider)*dy/(Np/divider); 
}

// configuration ------------------------------------------------------------------------

bool cell_on_a_ring(int cell_number_nx, int cell_number_ny)
{
	double cx = (cell_number_nx+0.5 - 0.5*SIZE_X)*dx;
	double cy = (cell_number_ny+0.5 - 0.5*SIZE_Y)*dy;
	
	return 	( (r_larm-dr)*(r_larm-dr) <= (cx*cx + cy*cy)) &&
			((cx*cx + cy*cy) <= (r_larm+dr)*(r_larm+dr) );	
}

bool cell_in_a_circle(int cell_number_nx, int cell_number_ny)
{
	double cx = (cell_number_nx+0.5 - 0.5*SIZE_X)*dx;
	double cy = (cell_number_ny+0.5 - 0.5*SIZE_Y)*dy;
	
	return ( sqrt(cx*cx + cy*cy) <= (r_larm+dr)*r_prop );
}

// impulse loading ----------------------------------------------------------------------

double temperature_impulse(double temperature, double mass)
{
	return sin(2.*M_PI*frand())*sqrt(-2.*(temperature*mass/mec2)*log(frand())); 
}

void load_annular_impulse(double x, double y,
	double mass, double Tx, double Ty, double Tz,
	double p0, double* px, double* py, double* pz)
{
	double center_x = 0.5*SIZE_X*dx;
	double center_y = 0.5*SIZE_Y*dy;
	double r = sqrt((x - center_x)*(x - center_x) 
				+ (y - center_y)*(y - center_y));
	
	*px = p0*( +(y - center_y)/r ) + temperature_impulse(Tx, mass); 
	*py = p0*( -(x - center_x)/r ) + temperature_impulse(Ty, mass);
	*pz = temperature_impulse(Tz, mass); 
}

void load_uniform_impulse(double x, double y,
	double mass, double Tx, double Ty, double Tz,
	double p0, double* px, double* py, double* pz)
{
	*px = temperature_impulse(Tx, mass);
	*pz = temperature_impulse(Ty, mass);
	*pz = temperature_impulse(Tz, mass); 	
}