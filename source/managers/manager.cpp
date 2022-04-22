#include "manager.hpp"

#include <omp.h>
#include <memory>

#include "time_manager.hpp"
#include "../command/command.hpp"
#include "../command/cmd_add_Bz0.hpp"
#include "../command/ionize_particles.hpp"
#include "../fields/fields_builder.hpp"
#include "../particles/particles_builder.hpp"
#include "../particles/particles_load.hpp"
#include "../constants.h"


void Manager::initializes()
{
	Fields_builder fields_builder;
	fields_ = std::move(fields_builder.build());
		
	#if there_are_fields
		#if there_are_Bz0
		settings_before_main_cycle.push_back(std::make_unique<Add_Bz0>(&fields_, Bz0));
		#endif
	#endif
	
	Particles_builder particles_builder;
	list_of_particles_ = particles_builder.build(fields_, settings_before_main_cycle);		

	// TODO: fabric on each_step_presets
	// TODO: variable set_time_distribution
	#if there_are_ions
		std::cout << "\tSetting each step presets..."
			<< "\n\t\tIonization "; 

		Particles* const ionized = list_of_particles_["ions"].get();
		Particles* const lost = list_of_particles_["buffer_electrons"].get();

		const int Np_il = ionized->get_parameters().Np();
		const int total_number_of_particles = get_number_of_particles_on_ring(Np_il);
	
		each_step_presets.push_front(std::make_unique<Ionize_particles>(
			ionized, lost,
			set_time_distribution(TINJ, total_number_of_particles),
			set_point_on_annulus,
			uniform_probability,
			load_annular_impulse
		));
	
		std::cout << "\n\t\tcheck : (&ionized=" << ionized << ", "
				  << "&lost=" << lost << ")";

		std::cout << "\n\t\tdone\n"
			<< "\tdone!" << std::endl;
	#endif
}


void Manager::calculates()
{
	std::cout << "\n\tProcessing main cycle..." << std::endl;
	Timer timer;
	timer.set_up();

	if (!settings_before_main_cycle.empty()) {
		for (const auto& command : settings_before_main_cycle)
			command->execute(0);
		settings_before_main_cycle.clear();
	}
	
	for (int t = 0; t < TIME; ++t) {	
	
		if (!each_step_presets.empty()) {
			
			// Сlearing the list of unnecessary elements.
			each_step_presets.remove_if(
				[t](const Command_up& command){
				return command->needs_to_be_removed(t);
			});

			// Executing the rest of the commands.
			for (const auto& command : each_step_presets)
				command->execute(t);
		}

	
		#if there_are_particles
		for (auto& [_, particles] : list_of_particles_) {
			particles->diagnose(t);
			//particles->push();
		}
		#endif
	
		#if there_are_fields
		fields_.diagnose(t);
		//fields_.propogate();
		#endif

		timer.tick(t);
 	}

	timer.elapsed();
}