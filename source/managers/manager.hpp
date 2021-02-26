#include "./fields_manager.hpp"
#include "./particles_manager.hpp"
#include "../constants.h"

#include <iostream>
#include <chrono>
#include <omp.h>

using namespace std;

#ifndef MANAGER_H
#define MANAGER_H

//#################################################################################################

class Manager {
private:
	Fields_manager 	  FM;
	//Particles_manager PM;

	//int TIME;

public:
	Manager(string initializer_) {
		// TODO: нужен сюда нормальный парсер, пока данные частично берутся из файла
		//		 constants.h, как это было раньше
		mkdir(test_name.c_str());

		FM.initialization(field_solver, field_configuration,
			test_name, field_diagnostics);
		//PM.initialization(particles_solvers, particles_configuration, 
		//	test_name, particles_diagnostics);
	}

	void Calculate() {

		auto start = chrono::system_clock::now();

		FM.add_Bz0(Bz0);
		FM.diagnose();
/*
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
*/
		auto end = chrono::system_clock::now();
		chrono::duration<double> elapsed = end - start;
		cout << "\n\n\truntime:\t" << elapsed.count() << "s\n\n" << endl;
	}

};

//#################################################################################################

#endif
