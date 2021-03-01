#ifndef MANAGER_H
#define MANAGER_H

//#################################################################################################

#include "./fields.hpp"
#include "./particles.hpp"
#include "../constants.h"

#include <iostream>
#include <chrono>
#include <omp.h>

using namespace std;


class Manager {
private:
	Fields fields;
	vector<Particles> particles;

	//int TIME;

public:
	Manager(string initializer_) {
		// TODO: нужен сюда нормальный парсер, пока данные частично берутся из файла
		//		 constants.h, как это было раньше
		mkdir(test_name.c_str());

		fields.initialization(field_solver, field_configuration,
			test_name, field_diagnostics);

		// TODO: сделать перебор по сортам, которые так быстренько инициализируются
		//		for (int s = SORT; s < NUM_OF_SORTS; ++s) {
		//		if (sort[s] == "protons") {

		// TODO: перебор сортов будет прямо здесь, а для каждой вызывается инициализатор
		// Particles temp
		// temp.initialization(...);
		// particles.push_back(temp)
		// particles.initialization(particles_solvers, particles_configuration, 
		
		//	test_name, particles_diagnostics);
	}

	void Calculate() {

		auto start = chrono::system_clock::now();

		fields.add_Bz0(Bz0);
		fields.diagnose();


		for (int t = 0; t < TIME; ++t) {
			
			fields.add_circular_current(particles[0], v_inj, Bz0, t);
				
			
			for (auto& sort : particles) {
				#pragma omp for num
				for (int i = 0; i < sort.amount(); ++i) {
			
					vector2 r0 = sort.element(i).r();
			
					sort.particle_push(i, E, B);
	
					sort.density_decomposition(i, r0, j);
			
					sort.boundaries_processing(i, SIZE_X*dx, SIZE_Y*dy);

					sort.diagnose();
				}
			}

			fields.propogate();	
			fields.diagnose();
		}


		auto end = chrono::system_clock::now();
		chrono::duration<double> elapsed = end - start;
		cout << "\n\n\truntime:\t" << elapsed.count() << "s\n\n" << endl;
	}

};

//#################################################################################################

#endif
