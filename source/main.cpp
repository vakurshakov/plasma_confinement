#include "2d_simulation.hpp"

void add_circular_current(vector3_field& j, class_particles& sort, double v_inj, double Bz);

int main()
{
	diagnostics_manager DM(head_folder, names_of_diagnostics);
	// solvers_manager SM(solvers, configuration);

/*
	// initialisation (%_vector3_fields)
	reflective_vector3_field E(SIZE_X, SIZE_Y);
	reflective_vector3_field B(SIZE_X, SIZE_Y);
	reflective_vector3_field j(SIZE_X, SIZE_Y);
*/
	protons el(np, Np, XY_distrib, vector2(0,0));

/*
	double energy = 0;
	
	// load Particle Distribution
	vector3 p0(0.3/sqrt(1-0.3*0.3), 0, 0);
	electrons beam_electrons(nb, Np, p0);

	p0.x() = - nb*p0.x();
	electrons plasma_electrons(np, Np, p0);

	std::vector<sort_of_particles> particles = {beam_electrons, plasma_electrons};

	// boundaries condition along the X and Y axes
	void (*Xboundaries_for)(particle&, double) = reflective_Xboundaries_for;
	void (*Yboundaries_for)(particle&, double) = reflective_Yboundaries_for;

	circular_current_init(j, ni, vi, mi, d);	

	for (int t = 0; t < TIME; ++t) {
		energy = 0;

		#pragma omp parallel shared(particles, E, B, j), num_threads(8)
		{	

			for (auto& sort : particles)	
				#pragma omp for // reduction(+: energy)
				for (int i = 0; i < sort.amount(); ++i) {
					vector2 r0 = sort.element(i).r();
					
					Boris_pusher(sort, sort.element(i), E, B); 
					
					Esirkepov_density_decomposition(sort, sort.element(i), r0, j);
					
					Xboundaries_for(sort.element(i), SIZE_X*dx);
					Yboundaries_for(sort.element(i), SIZE_Y*dy);
			}
		}
		
		FDTD_2D(E, B, j);
		
	//#### diagnostics ################################################################
	
		#pragma omp parallel shared(E, B) reduction(+: energy), num_threads(8)
		{
			#pragma omp for 
			for (int y = 0; y < SIZE_Y; ++y) {
			for (int x = 0; x < SIZE_X; ++x) {
				energy += 0.5*( E.x(y,x)*E.x(y,x) + E.y(y,x)*E.y(y,x) + E.z(y,x)*E.z(y,x) +
						   	    B.x(y,x)*B.x(y,x) + B.y(y,x)*B.y(y,x) + B.z(y,x)*B.z(y,x) )*dx*dy;
			}
			}
		}
	
		fout_energy << energy << std::endl;
		fout_field_at << E.x(SRC_POS_Y, SRC_POS_X) << std::endl;
		if (t % 5 == 0) { write_vector3_field(E, X, fout_field); };

	//##################################################################################
	
	}
	
	phase_px_x(particles, -1, 1, 0, SIZE_X, fout_phase);
	
	add_circular_current(j, injected_ion, v_inj, Bz);

	write_vector3_field(j, X, fout_field);
	write_vector3_field(j, Y, fout_field);
	
	fout_field.close();
	fout_field_at.close();
	fout_energy.close();
	fout_phase.close();

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "\truntime:\t" << elapsed.count() << "s\n" << std::endl << std::endl;
*/
	return 0;
}
