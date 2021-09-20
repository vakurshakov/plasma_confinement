#include "./manager.hpp"

#include <memory>

#include <omp.h>

#include "./fields_builder.hpp"
#include "./particles_builder.hpp"
#include "./time_manager.hpp"
#include "../command/command_add_Bz0.hpp"
#include "../constants.h"


void Manager::initializes()
{
	Fields_builder fields_builder;
	fields_ = fields_builder.build();
		
	#if there_are_fields
		#if there_are_Bz0
		set_up_commands.push_front(make_unique<Add_Bz0>(&fields_, Bz0));
		#endif
	#endif
	
	// Нужно забросить поля, чтобы задать пуш-команды
	Particles_builder particles_builder(&fields_);
	list_of_particles_ = particles_builder.build();	
}


void Manager::calculates()
{
	Timer timer;
	timer.set_up();

	if (!set_up_commands.empty()) {
		for (auto& command : set_up_commands_) {
			command->execute();
		}
	}
	
	for (decltype(TIME) t = 0; t < TIME; ++t) {	

		#if there_are_particles
		for (auto& particles : list_of_particles_) {
			particles.diagnose(t);
			particles.push();
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