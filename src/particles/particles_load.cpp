#include "particles_load.hpp"

#include "src/pch.h"
#include "src/utils/random_number_generator.hpp"

void fill_randomly(int sequential_number, int Np,
		int cell_number_nx, int cell_number_ny,
		double* x, double* y) {
	static auto distribution = std::uniform_real_distribution(0.0, 1.0);
	
	*x = (cell_number_nx + distribution(Random_generator::get())) * dx;  
	*y = (cell_number_ny + distribution(Random_generator::get())) * dy;  
}

void fill_periodically(int sequential_number, int Np,
	int cell_number_nx, int cell_number_ny,
	double* x, double* y) {
	/** 
	 * i%Np -- номер от нуля до Np-1 частицы в одной ячейке
	 * i/Np -- список индексов, эквивалентный перебору ячеек
	 * индекс ячейки по Ox: (i/Np % divider), Oy: (i/Np / divider).
	 *
	 * Cколько по длинне в ячейку влазит? Это надо искать делители Np
	 * при том лучше, конечно, от половины Np начинать в обе стороны искать
	 * Пока эта проблема решена так: divider -- число, показывающее 
	 * сколько частиц будет уложено в одну ячейку вдоль оси Ox; 
	 */
	
	int divider = 2;
	*x = cell_number_nx * dx + ((sequential_number % Np) % divider) * dx / divider;
	*y = cell_number_ny * dy + ((sequential_number % Np) / divider) * dy / (Np / divider); 
}


double temperature_impulse(double temperature, double mass) {
	static auto distribution = std::uniform_real_distribution(0.0, 1.0);

	static const double mec2 = 511.0;
	return sin(2.0 * M_PI * distribution(Random_generator::get())) *
		sqrt(-2.0 * (temperature * mass / mec2)*log(distribution(Random_generator::get()))); 
}

void load_uniform_impulse(double x, double y,
		double mass, double Tx, double Ty, double Tz,
		double p0, double* px, double* py, double* pz) {
	*px = temperature_impulse(Tx, mass);
	*py = temperature_impulse(Ty, mass); 
	*pz = temperature_impulse(Tz, mass); 	
}