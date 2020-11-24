#include "2d_simulation.hpp"

//-------- functions -------------------------------------------------------------------
double frand() { return ((double)rand()/RAND_MAX); }

int main()
{
	auto start = std::chrono::system_clock::now();

	std::ofstream fout_field, fout_field_at, fout_energy; //fout_particles;

	fout_field.open("diagnostics/fields_configuration/2D_anim._values.txt");
	fout_field << TIME << std::endl;
	fout_field << SIZE_X << " " << SIZE_Y << " " << std::endl;
	fout_field << std::setprecision(10) << std::fixed;

	fout_field_at.open("diagnostics/fields_configuration/field_at.txt");
	fout_field_at << TIME << std::endl;
	fout_field_at << std::setprecision(10) << std::fixed;

	//fout_particles.open("diagnostics/particles/particles_data.txt");
	//fout_particles << std::setprecision(10) << std::fixed;
	//fout_particles << (size_t)density_of_electorns*SIZE_X*SIZE_Y << std::endl;
	//fout_particles <<  TIME << "," << dt << std::endl;
	//fout_particles << SIZE_X*dx << "," << SIZE_Y*dy << std::endl;

	fout_energy.open("diagnostics/energy_conservation/energy_values.txt");
	fout_energy << TIME << std::endl;
	fout_energy << std::setprecision(20) << std::fixed;

	// initalization (%_vector3_fields)
	periodic_vector3_field E(SIZE_X, SIZE_Y);
	periodic_vector3_field B(SIZE_X, SIZE_Y);
	periodic_vector3_field j(SIZE_X, SIZE_Y);

	double energy = 0;
	
	// initalization (Load Particle Distribution)
	srand(time(NULL));
	sort_of_particles electrons(-e, me, second_order_spline, spline_width);

	std::cout << "\n\tload of particles:\n" << std::endl;

	int err = 0;
	for (size_t i = 0; i < (size_t)density_of_electorns*SIZE_X*SIZE_Y + err; ++i) {

		double x = frand()*SIZE_X*dx; 
		double y = frand()*SIZE_Y*dy; 
		
		double px = sin(2*3.14159*frand())*sqrt(-2*(Temperature*electrons.m()/mec2)*log(frand()));
		double py = sin(2*3.14159*frand())*sqrt(-2*(Temperature*electrons.m()/mec2)*log(frand()));
		double pz = 0; //sin(2*3.14159*frand())*sqrt(-2*(Temperature*electrons.m()/mec2)*log(frand()));
		
		if (std::isinf(px) | std::isinf(py) | std::isinf(pz)) { 
			std::cout << "\t\tpx, py or pz is inf!" << std::endl;
			++err;
			continue;
		}
		
		vector2 r(x, y);
		vector3 p(px, py, pz);		
	
		particle electron(r, p);
		electrons.particles().push_back(electron);		

		energy += sqrt(electrons.m()*electrons.m() + p.dot(p))*dx*dy/density_of_electorns;
	}

	std::cout << "\n\t" << electrons.amount() << " particles has been loaded;\n\n\trunning main cycle:\n" << std::endl;

	fout_energy << energy << std::endl;

	// boundaries condition along the X and Y axes
	void (*Xboundaries_for)(particle&, double) = periodic_Xboundaries_for;
	void (*Yboundaries_for)(particle&, double) = periodic_Yboundaries_for;

	// main circle
	for (size_t t = 0; t < TIME; ++t) {
		energy = 0;

		#pragma omp parallel shared(electrons, E, B, j, dt, dx, dy, density_of_electorns), num_threads(8)
		{	// Particle Interpolation & Solving Particle EQM
		 	// (E, B) -> Fp -> (xp, pp),
			#pragma omp for reduction(+: energy)
			for (auto it = electrons.particles().begin(); it != electrons.particles().end(); ++it) {
				vector2 r0 = (*it).r();
				
				// go through all cells, calculate local fields & push them
				Boris_pusher(electrons, (*it), E, B, dt, dx, dy); 
				assert(((*it).r().x() - r0.x() <= dx) && ((*it).r().y() - r0.y() <= dy));
				
				// Extrapolate to Grid
				// (xp, pp) -> (rho, j),
				Esirkepov_density_decomposition(electrons, (*it), r0, j, dt, dx, dy);
				
				// boundary condition for the whole box
				Xboundaries_for((*it), SIZE_X*dx);
				Yboundaries_for((*it), SIZE_Y*dy);
				
				energy += sqrt(electrons.m()*electrons.m() + (*it).p().dot((*it).p()))*dx*dy/density_of_electorns;
			}
		}
		
		// Solve Maxwell's Equation
		// (rho, j) -> (E, B),
		FDTD_2D(E, B, j, dt, dx, dy);
		
		#pragma omp parallel shared(E, B, dx, dy) reduction(+: energy), num_threads(8)
		{
			#pragma omp for 
			for (int y = 0; y < E.size_y(); ++y) {
			for (int x = 0; x < E.size_x(); ++x) {
				energy += 0.5*(E.x(y,x)*E.x(y,x) + E.y(y,x)*E.y(y,x) + E.z(y,x)*E.z(y,x) +
						   	   B.x(y,x)*B.x(y,x) + B.y(y,x)*B.y(y,x) + B.z(y,x)*B.z(y,x) )*dx*dy;
			}
			}
		}

		fout_energy << energy << std::endl;
		fout_field_at << E.x(SRC_POS_Y, SRC_POS_X) << std::endl;
		//write_vector3_field(B, Z, fout_field);
	}

	// фотография полей на последнем шаге по времени
	write_vector3_field(E, X, fout_field);
	write_vector3_field(E, Y, fout_field);
	write_vector3_field(E, Z, fout_field);

	write_vector3_field(B, X, fout_field);
	write_vector3_field(B, Y, fout_field);
	write_vector3_field(B, Z, fout_field);

	// закрываем запись файлов
	fout_field.close();
	fout_field_at.close();
	fout_energy.close();
	//fout_particles.close();

	// комментарии к пуску
	std::cout << "\n\n\tCOMMENT SECTION:" << std::endl;
	std::cout << "\tt:\t" << TIME << ",\tSIZE_X:\t" << SIZE_X << ",\tSIZE_Y:\t" << SIZE_Y << std::endl;
	std::cout << "\tdensity:\t" << density_of_electorns << std::endl;
	std::cout << "\tcoordinate:\t(SPX, SPY)" << std::endl;

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "\truntime:\t" << elapsed.count() << "s\n" << std::endl;
	
	return 0;
}