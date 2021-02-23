#include "2d_simulation.hpp"

using namespace std;
using up_v3f = unique_ptr<vector3_field>;


class Manager {
private:
	up_v3f E, B, j;
	vector<class_particles> particles;

	solvers_manager 	SM;	
	diagnostics_manager DM;

	//int TIME;

public:
	Manager(string initializer_) {
		// NOTE: пока я сделаю так, потом нужно будет добавить 
		//		 парсер, чтобы он обрабатывал файл с начальными данными

		SM.initialisation(E, B, j, particles, solvers, configuration);
		DM.initialisation(head_folder, names_of_diagnostics);
	}


	void Calculate() {
		auto start = std::chrono::system_clock::now();

		for (int t = 0; t < TIME; ++t) {
			//#pragma omp parallel shared(SM.data()), num_threads(8)
			#pragma omp parallel shared(particles, E, B, j), num_threads(8)
			{	
	
				for (auto& sort : particles)	
					#pragma omp for
					for (int i = 0; i < sort.amount(); ++i) {
						
						vector2 r0 = sort.element(i).r();
						
						SM.Particle_push(sort, sort.element(i), E, B);

						SM.Density_decomposition(sort, sort.element(i), r0, j);
						
						SM.Boundaries_processing(sort.element(i), SIZE_X*dx, SIZE_Y*dy);
			}
			
			SM.Propogate_fields(E, B, j);
			
			//DM.diagnose_t( E, B, j, particles );
		}
	
		//DM.diagnose( E, B, j, particles );

		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed = end - start;
		std::cout << "\n\n\truntime:\t" << elapsed.count() << "s\n\n" << std::endl;
	}

};