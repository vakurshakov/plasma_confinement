#include "2d_simulation.hpp"

int main()
{

	auto start = std::chrono::system_clock::now();


	// setup settings
	std::cout << "\n\n\tCOMMENT SECTION:" << std::endl;
	std::cout << "\tt:\t" << TIME << ",\tSIZE_X:\t" << SIZE_X << ",\tSIZE_Y:\t" << SIZE_Y << std::endl;
	std::cout << "\tNp:\t" << Np << std::endl;

//---- diagnostics -----------------------------------------------------------------

	
	//#### making directiories #########################################################
	
	std::string current_path = "../diagnostics/Beam" +
								std::to_string(SIZE_X) + "cellX" +
								std::to_string(SIZE_Y) + "cellY" +  
								std::to_string(int(2*Np)) + "Np" +
								"Rand" +
								"Tx" + std::to_string(Tx).substr(0, 3) +
								"Ty" + std::to_string(Ty).substr(0, 3) +
								"Tz" + std::to_string(Tz).substr(0, 3) +
								"_nb" + std::to_string(nb).substr(0, 4) +
								 "np" + std::to_string(np).substr(0, 4) ;

	mkdir(current_path.c_str());
	mkdir((current_path + "/energy_conservation" ).c_str());
	mkdir((current_path + "/fields_configuration").c_str());
	mkdir((current_path + "/phase" ).c_str());	

	//##################################################################################
	
	
	std::ofstream fout_field, fout_field_at, fout_energy, fout_phase;
	
	fout_field.open((current_path + "/fields_configuration/2D_anim._values.txt").c_str());
	fout_field << TIME << std::endl;
	fout_field << SIZE_X << " " << SIZE_Y << " " << std::endl;
	fout_field << std::setprecision(10) << std::fixed;

	fout_field_at.open((current_path + "/fields_configuration/field_at.txt").c_str());
	fout_field_at << TIME << " " << dt << std::endl;
	fout_field_at << std::setprecision(10) << std::fixed;

	fout_energy.open((current_path + "/energy_conservation/energy_values.txt").c_str());
	fout_energy << TIME << " " << dt << std::endl;
	fout_energy << std::setprecision(20) << std::fixed;
	

	fout_phase.open((current_path + "/phase/phase_data.txt").c_str());
	
//----------------------------------------------------------------------------------
	
	// initalization (%_vector3_fields)
	reflective_vector3_field E(SIZE_X, SIZE_Y);
	reflective_vector3_field B(SIZE_X, SIZE_Y);
	reflective_vector3_field j(SIZE_X, SIZE_Y);

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

	circular_current_init(j, ni, vi, d);	

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

	// picture of a field condition at the last step
	//write_vector3_field(E, X, fout_field);
	//write_vector3_field(E, Y, fout_field);
	//write_vector3_field(E, Z, fout_field);

	//write_vector3_field(B, X, fout_field);
	//write_vector3_field(B, Y, fout_field);
	//write_vector3_field(B, Z, fout_field);

	phase_px_x(particles, -1, 1, 0, SIZE_X, fout_phase);
	
	fout_field.close();
	fout_field_at.close();
	fout_energy.close();
	fout_phase.close();

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "\truntime:\t" << elapsed.count() << "s\n" << std::endl << std::endl;

	return 0;
}
