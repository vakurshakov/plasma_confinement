#include "2d_simulation.hpp"

int main()
{

	auto start = std::chrono::system_clock::now();

	
	//---- diagnostics -----------------------------------------------------------------

	
	//#### making directiories #########################################################
	
	std::string current_path = "../diagnostics/InfTest" +
								std::to_string(SIZE_X) + "cell" + 
								std::to_string(int(Np)) + "Np" +
								"Rand" +
								"Tx" + std::to_string(Tx).substr(0, 3) +
								"Ty" + std::to_string(Tx).substr(0, 3) +
								"Tz" + std::to_string(Tx).substr(0, 3) +
								"OscX";

	mkdir(current_path.c_str());
	mkdir((current_path + "/energy_conservation" ).c_str());
	mkdir((current_path + "/fields_configuration").c_str());
	//mkdir((current_path + "/particles" ).c_str());	

	//##################################################################################
	
	
	std::ofstream fout_field, fout_field_at, fout_energy, fout_particles;

	
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
	

	// fout_particles.open((current_path + "/particles/particles_data.txt").c_str());
	// fout_particles << TIME << " " << dt << std::endl;
	// fout_particles << SIZE_X << " " << SIZE_Y << std::endl;
	// fout_particles << std::setprecision(10) << std::fixed;

	//----------------------------------------------------------------------------------
	
	// initalization (%_vector3_fields)
	periodic_vector3_field E(SIZE_X, SIZE_Y);
	periodic_vector3_field B(SIZE_X, SIZE_Y);
	periodic_vector3_field j(SIZE_X, SIZE_Y);

	double energy = 0;
	
	// load Particle Distribution
	srand(time(NULL));
	sort_of_particles electrons(-e, me, Np, second_order_spline, spline_width);

	std::cout << "\n\tload of particles:\n" << std::endl;

	int err = 0;
	for (size_t i = 0; i < (size_t)electrons.Np()*SIZE_X*SIZE_Y + err; ++i) {

		double x = frand()*SIZE_X*dx; // (8. + 0.5*frand()*SIZE_X)*dx; // (3./8. + 1./4.*frand())*SIZE_X*dx;  
		double y = frand()*SIZE_Y*dy; // (8. + 0.5*frand()*SIZE_Y)*dy; // (3./8. + 1./4.*frand())*SIZE_Y*dy;  
		
		double px = 0.01*cos(2.*M_PI*x/(SIZE_X*dx)) + sin(2.*M_PI*frand())*sqrt(-2.*(Tx*electrons.m()/mec2)*log(frand())); // 0.01*cos(2.*M_PI*x/(SIZE_X*dx)) + 
		double py = sin(2.*M_PI*frand())*sqrt(-2.*(Ty*electrons.m()/mec2)*log(frand()));
		double pz = 0; // sin(2.*M_PI*frand())*sqrt(-2.*(Tz*electrons.m()/mec2)*log(frand()));

		if (std::isinf(px) | std::isinf(py) | std::isinf(pz)) { 
			std::cout << "\t\tpx, py or pz is inf!" << std::endl;
			++err;
			continue;
		}
		
		vector2 r(x, y);
		vector3 p(px, py, pz);		
	
		particle electron(r, p);
		electrons.particles().push_back(electron);		

		energy += sqrt( (electrons.m()*electrons.m() + p.dot(p)) - electrons.m() )*dx*dy/electrons.Np();
	}

	std::cout << "\n\t" << electrons.amount() << " particles has been loaded;\n\n\trunning main cycle:\n" << std::endl;

	fout_energy << energy << std::endl;

	// boundaries condition along the X and Y axes
	void (*Xboundaries_for)(particle&, double) = periodic_Xboundaries_for;
	void (*Yboundaries_for)(particle&, double) = periodic_Yboundaries_for;

	for (size_t t = 0; t < TIME; ++t) {
		energy = 0;

		#pragma omp parallel shared(electrons, E, B, j), num_threads(8)
		{	
			#pragma omp for reduction(+: energy)
			for (auto it = electrons.particles().begin(); it != electrons.particles().end(); ++it) {
				vector2 r0 = (*it).r();
				
				Boris_pusher(electrons, (*it), E, B, dt, dx, dy); 
				
				Esirkepov_density_decomposition(electrons, (*it), r0, j, dt, dx, dy);
				
				Xboundaries_for((*it), SIZE_X*dx);
				Yboundaries_for((*it), SIZE_Y*dy);
				
				energy += sqrt( ( electrons.m()*electrons.m() + (*it).p().dot((*it).p()) ) - electrons.m() )*dx*dy/electrons.Np();
			}
		}
		fout_energy << energy << " ";
	
		FDTD_2D(E, B, j, dt, dx, dy);
		
		#pragma omp parallel shared(E, B) reduction(+: energy), num_threads(8)
		{
			#pragma omp for 
			for (int y = 0; y < SIZE_Y; ++y) {
			for (int x = 0; x < SIZE_X; ++x) {
				energy += 0.5*(E.x(y,x)*E.x(y,x) + E.y(y,x)*E.y(y,x) + E.z(y,x)*E.z(y,x) +
						   	   B.x(y,x)*B.x(y,x) + B.y(y,x)*B.y(y,x) + B.z(y,x)*B.z(y,x) )*dx*dy;
			}
			}
		}
	
		fout_energy << energy << std::endl;
		fout_field_at << E.x(SRC_POS_Y, SRC_POS_X) << std::endl;
		if (t % 5 == 0 ) { write_vector3_field(E, X, fout_field); }
	}

	// picture of a field condition at the last step
	//write_vector3_field(E, X, fout_field);
	//write_vector3_field(E, Y, fout_field);
	//write_vector3_field(E, Z, fout_field);

	//write_vector3_field(B, X, fout_field);
	//write_vector3_field(B, Y, fout_field);
	//write_vector3_field(B, Z, fout_field);

	fout_field.close();
	fout_field_at.close();
	fout_energy.close();
	//fout_particles.close();

	// setup settings
	std::cout << "\n\n\tCOMMENT SECTION:" << std::endl;
	std::cout << "\tt:\t" << TIME << ",\tSIZE_X:\t" << SIZE_X << ",\tSIZE_Y:\t" << SIZE_Y << std::endl;
	//std::cout << "\tdensity:\t" << electrons.Np() << std::endl;
	std::cout << "\tcoordinate:\t(SPX, SPY)" << std::endl;

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "\truntime:\t" << elapsed.count() << "s\n" << std::endl << std::endl;

	return 0;
}
