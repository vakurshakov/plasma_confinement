void fill_randomly(int sequential_number, int Np,
	int cell_number_cell_number_nx, int cell_number_ny,
	double* x, double* y);

void fill_periodically(int sequential_number, int Np,
	int cell_number_nx, int cell_number_ny,
	double* x, double* y);

// configuration ------------------------------------------------------------------------

bool cell_on_a_ring(int cell_number_nx, int cell_number_ny);
int  get_number_of_particles_on_ring(int Np);

bool cell_in_a_circle(int cell_number_nx, int cell_number_ny);
int  get_number_of_particles_in_circle(int Np);

int  get_number_of_particles_on_circle_segment(int Np);

// impulse loading ----------------------------------------------------------------------

double temperature_impulse(double temperature, double mass);

void load_annular_impulse(double x, double y,
	double mass, double Tx, double Ty, double Tz,
	double p0, double* px, double* py, double* pz);

void load_uniform_impulse(double x, double y,
	double mass, double Tx, double Ty, double Tz,
	double p0, double* px, double* py, double* pz);