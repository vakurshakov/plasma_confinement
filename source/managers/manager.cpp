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
	Particles_builder particles_builder(&fields_);
	list_of_particles_ = std::move(particles_builder.build());

	#if there_are_ions
		// Cейчас просто засовываем в класс ионизации нужные функции, потом разберёмся
		Ionization_up ionization = std::make_unique<Ionization>(
			cell_on_a_ring, fill_randomly, load_annular_impulse, uniform_density);
		
		Particles& ionized = list_of_particles_["ions"];
		Particles& lost = list_of_particles_["buffer_electrons"];

		each_step_presets.push_front(std::make_unique<Create_particles>(
			std::move(ionization), &ionized, &lost));
	#endif
}


void Manager::calculates()
{
	Timer timer;
	timer.set_up();

	if (!settings_before_main_cycle.empty()) {
		for (auto& command : settings_before_main_cycle) {
			command->execute();
		}
	}
	
	for (int t = 0; t < TIME; ++t) {	

		if (!each_step_presets.empty()) {
			for (auto& command : each_step_presets) {
				command->execute();
			}
		}	
	
		#if there_are_particles
		for (auto& particles : list_of_particles_) {
			particles.second.diagnose(t);
			particles.second.push();
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