#include "manager.hpp"

#include <omp.h>
#include <memory>

#include "time_manager.hpp"
#include "../command/command.hpp"
#include "../command/cmd_add_Bz0.hpp"
#include "../command/cmd_create_particles.hpp"
#include "../fields/fields_builder.hpp"
#include "../particles/particles_builder.hpp"
#include "../particles/add_ionization.hpp"
#include "../particles/particles_load.hpp"
#include "../constants.h"


using Ionization_up = std::unique_ptr<Ionization>;


void Manager::initializes()
{
	Fields_builder fields_builder;
	fields_ = std::move(fields_builder.build());
		
	#if there_are_fields
		#if there_are_Bz0
		settings_before_main_cycle.push_front(std::make_unique<Add_Bz0>(&fields_, Bz0));
		#endif
	#endif
	
	// Нужно забросить поля, чтобы задать пуш-команды
	Particles_builder particles_builder(fields_);
	list_of_particles_ = particles_builder.build();
	
	#if there_are_ions
		std::cout << "\tSetting each step presets..."
			<< "\n\t\tIonization "; 
		
		Particles* const ionized = list_of_particles_["ions"].get();
		Particles* const lost = list_of_particles_["buffer_electrons"].get();

		int Np = ionized->get_parameters().Np();
		int total_number_of_particles_to_load = 1; // = static_cast<int>(
		//	Np*M_PI*((r_larm+dr)*(r_larm+dr) - (r_larm-dr)*(r_larm-dr))/(dx*dy));

		if ( total_number_of_particles_to_load/TINJ == 0 ) {
			std::cout << "\n\t\twhat(): error in tot_num/tinj" << std::endl;
		}
		else {
			std::cout << "\n\t\tThere would be " << total_number_of_particles_to_load
				<< " loaded particles (" << total_number_of_particles_to_load/TINJ << " each_step)";

			// Cейчас просто засовываем в класс ионизации нужные функции, потом разберёмся
			Ionization_up ionization = std::make_unique<Ionization>(
				total_number_of_particles_to_load, TINJ,
				set_point_on_circle, uniform_probability, load_annular_impulse);
		
			// ionized->points_.reserve(total_number_of_particles_to_load);
			// lost->points_.reserve(total_number_of_particles_to_load);

			each_step_presets.push_front(std::make_unique<Create_particles>(
				ionization, ionized, lost));
		
			std::cout << "\n\t\tcheck : (&ionization=" << ionization.get() << ", "
					<< "&ionized=" << ionized << ", "
					<< "&lost=" << lost << ")";
			
			std::cout << "\n\t\tdone\n"
			<< "\tdone!" << std::endl;
		}
	#endif
}


void Manager::calculates()
{
	std::cout << "\n\tProcessing main cycle..." << std::endl;
	Timer timer;
	timer.set_up();

	if (!settings_before_main_cycle.empty()) {
		for (auto& command : settings_before_main_cycle) {
			command->execute(0);
		}
	}
	
	for (int t = 0; t < TIME; ++t) {	
	
		if (!each_step_presets.empty()) {
			for (auto& command : each_step_presets) {
				command->execute(t);
			}
		}

		#if there_are_particles
		for (auto& particles : list_of_particles_) {
			particles.second->diagnose(t);
			particles.second->push();
		}
		#endif
	
		#if there_are_fields
		fields_.diagnose(t);
		fields_.propogate();
		#endif			
		
		timer.tick(t);
	}

	timer.elapsed();
}