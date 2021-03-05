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

	//int TIME;

public:
	Manager(string initializer_) {

		/*
			TODO: нужен сюда нормальный парсер, пока данные частично берутся из файла
				constants.h, как это было раньше
		*/
		mkdir(test_name.c_str());

		fields_.initialize(field_solver, field_configuration,
			test_name, field_diagnostics);

		/*
			TODO: подумать как избавиться от if-else повторений
		*/

		for (auto description : species) {
			if ( description.first == "Electrons" ) {

				Electrons temp;
				
				temp.initialize(particles_solvers, description.second[0], 
					test_name, description.second[1]);

				particles_.push_back(move(temp));
			}
			else if ( description.first == "Protons -d" ) {

				Protons temp(ni);

				particles_.push_back(move(temp));
			}
		}
	}

	void Calculate() {

		auto start = chrono::system_clock::now();

		fields_.add_Bz0(Bz0);
		
		for (int t = 0; t < TIME; ++t) {
			
			fields_.add_circular_current(particles_[1], v_inj, Bz0, t);
		
			#pragma omp parallel num_threads(8)
			{
				for (auto& sort : particles_) {
					#pragma omp for
					for (int i = 0; i < sort.amount(); ++i) {
				
						vector2 r0 = sort.element(i).r();
				
						sort.particle_push(i, fields_.E(), fields_.B());
			
						sort.density_decomposition(i, r0, fields_.j());
				
						sort.boundaries_processing(i, SIZE_X*dx, SIZE_Y*dy);

						// TODO: что-то не так с файловой системой, нужно будет решить
						//sort.diagnose();
					}
				}
			}
		
			fields_.diagnose();

			fields_.propogate();
		}

		auto end = chrono::system_clock::now();
		chrono::duration<double> elapsed = end - start;
		cout << "\n\n\truntime:\t" << elapsed.count() << "s\n\n" << endl;
	}

};

//#################################################################################################

#endif
