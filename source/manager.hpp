#include "2d_simulation.hpp"

using namespace std;
using up_v3f = unique_ptr<vector3_field>;


class Manager {
private:

	up_v3f E, B, j;
	// NOTE: class fields_manager F () 
	vector<class_particles> particles;
	// NOTE: перенести в частицы толкатель и диагностики для отдельного сорта

	solvers_manager 	SM;	
	diagnostics_manager DM;

	//	particles_manager PM;
	//	fields_manager 	  FM;

	//int TIME;

public:
	Manager(string initializer_) {
		// NOTE: пока я сделаю так, потом нужно будет добавить 
		//		 парсер, чтобы он обрабатывал файл с начальными данными

		SM.initialisation(E, B, j, particles, solvers, configuration);
		DM.initialisation(head_folder, names_of_diagnostics);
	}


	void Calculate() {

		//FM.Ex(y,x) = 1;
		PM.push( &FM )

		auto start = std::chrono::system_clock::now();

		SM.add_Bz0(B, Bz0);

		for (int t = 0; t < TIME; ++t) {
			// ADDITIONAL SECTION
			SM.add_circular_current(j, particles[0], v_inj, Bz0, t);
				
		/*
			for (auto& sort : PM.particles())	
					#pragma omp for
					for (int i = 0; i < sort.amount(); ++i) {
						
						vector2 r0 = sort.element(i).r();
						
						PM.Particle_push(sort, sort.element(i), E, B);

						PM.Density_decomposition(sort, sort.element(i), r0, j);
						
						PM.Boundaries_processing(sort.element(i), SIZE_X*dx, SIZE_Y*dy);
					
					sort.diagnose();
				}
			}
			
			FM.Propogate_fields();
			
		*/
			DM.diagnose_t(E, B, j, particles);
		}
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed = end - start;
		std::cout << "\n\n\truntime:\t" << elapsed.count() << "s\n\n" << std::endl;
	}


};
