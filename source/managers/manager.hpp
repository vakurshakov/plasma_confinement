#include "./fields_manager.hpp"
#include "./particles_manager.hpp"
#include "../constants.h"

using namespace std;

#ifndef MANAGER_H
#define MANAGER_H

//#################################################################################################

class Manager {
private:
	Fields_manager 	  FM;
	Particles_manager PM;

	//int TIME;

public:
	Manager(string initializer_) {
		// TODO: нужен сюда нормальный парсер, пока данные частично берутся из файла
		//		 constants.h, как это было раньше

		FM.initialisation(field_solver, field_configuration);
		PM.initialisation(particles_solvers, particles_configuration);
	}

/*
	void Calculate() {

		auto start = std::chrono::system_clock::now();

		SM.add_Bz0(B, Bz0);

		for (int t = 0; t < TIME; ++t) {
			// ADDITIONAL SECTION
			SM.add_circular_current(j, particles[0], v_inj, Bz0, t);
				
		
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
			
		
		}
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed = end - start;
		std::cout << "\n\n\truntime:\t" << elapsed.count() << "s\n\n" << std::endl;
	}
*/

};

//#################################################################################################

#endif
