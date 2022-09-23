#ifndef SRC_PARTICLES_PARTICLES_LOAD_HPP
#define SRC_PARTICLES_PARTICLES_LOAD_HPP

// TODO: impulse and coordinate loader

void fill_randomly(int sequential_number, int Np,
	int cell_number_cell_number_nx, int cell_number_ny,
	double* x, double* y);

void fill_periodically(int sequential_number, int Np,
	int cell_number_nx, int cell_number_ny,
	double* x, double* y);

double temperature_impulse(double temperature, double mass);

void load_uniform_impulse(double x, double y,
	double mass, double Tx, double Ty, double Tz,
	double p0, double* px, double* py, double* pz);

#endif  // SRC_PARTICLES_PARTICLES_LOAD_HPP
