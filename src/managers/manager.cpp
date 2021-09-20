#include "./manager.hpp"

#include <memory>

#include <omp.h>

#include "./fields_builder.hpp"
#include "./particles_builder.hpp"
#include "./time_manager.hpp"
#include "../command/command.hpp"
#include "../command/cmd_add_Bz0.hpp"
#include "../constants.h"


void Manager::initializes()
{
	Fields_builder fields_builder;
	fields_ = fields_builder.build();
		
	#if there_are_fields
		#if there_are_Bz0
		fields_setting_commands_.push_front(std::make_unique<Add_Bz0>(&fields_, Bz0));
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

	if (!fields_setting_commands_.empty()) {
		for (auto& command : fields_setting_commands_) {
			command->execute();
		}
	}
	
	for (int t = 0; t < TIME; ++t) {	

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