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
	Fields fields_;
	vector<Particles> particles_;

public:
	Manager(string initializer_) {

		/*
			TODO: нужен сюда нормальный парсер, пока данные частично берутся из файла
				constants.h, как это было раньше
		*/
		
		fields_.initialize(field_solver, field_configuration,
			dir_name, field_diagnostics);

		/*
			TODO: подумать как избавиться от if-else повторений
		*/

		
		#if there_are_particles
		for (auto description : species) {
			if ( description.first == "Electrons" ) {

				Electrons temp;
				
				temp.initialize("Electrons", particles_solvers, description.second[0],
					dir_name, description.second[1]);

				particles_.push_back(move(temp));
			}

			else if ( description.first == "Ions" ) {

				Ions temp;
				
				temp.initialize("Ions", particles_solvers, description.second[0],
					dir_name, description.second[1]);

				particles_.push_back(move(temp));
			}
		
		}
		#endif
	}

	void Calculate() {

		auto start = chrono::system_clock::now();

		#if there_are_Bz0
		fields_.add_Bz0(Bz0);
		#endif
			

		for (int t = 0; t < TIME; ++t) {
		
			#if there_are_current_add
			particles_[0].add_ion_current(t);
			#endif
			
			#if there_are_particles
			for (auto& sort : particles_) {
				#pragma omp parallel for num_threads(THREAD_NUM)
				for (int i = 0; i < sort.amount(); ++i) {
			
					vector2 r0 = sort.element(i).r();
					
					sort.particle_push(i, fields_.E(), fields_.B());
			
					sort.density_decomposition(i, r0, fields_.j());
				
					sort.boundaries_processing(i, SIZE_X*dx, SIZE_Y*dy);
				}

				// TODO: реализовать частицы и поля БЕЗ диагностик
				#if particles_are_diagnosed
				if (t % diagnose_time_step == 0) {
					sort.diagnose();
				}
				#endif
				
			}
			#endif
		
			#if fields_are_diagnosed
			if ( t % diagnose_time_step == 0 ) {
				fields_.diagnose();
			}
			#endif

			fields_.propogate();			
		
			if ( t % (TIME/10) == 0 ) {
				if ( t == 0 ) cout << endl;

				auto time = chrono::system_clock::now();
				chrono::duration<double> elapsed = time - start;
				
				cout << "\t[";
				for (int n = 0; n <= TIME; n+= TIME/10) {
					if ( n <= t ) { cout << "#"; }
					else { cout << " "; }
				}
				cout << "]\t" << elapsed.count() << "s" << endl;
			}	
		}
			
		auto end = chrono::system_clock::now();
		chrono::duration<double> elapsed = end - start;
		cout << "\n\n\truntime:\t" << elapsed.count() << "s\n" << endl;
	}

};

//#################################################################################################

#endif
