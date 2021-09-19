#include <vector>
#include <memory>
#include <chrono>

#include <omp.h>

#include "./command.hpp"
#include "./managers/entities_builder.hpp"
#include "./managers/fields.hpp"
#include "./managers/particles.hpp"
#include "./constants.h"


using command_up = unique_ptr<Command>;


int main()
{
	// Необходимые для работы cущности
	Fields fields;
	std::forward_list<Particles> particles;

	Timer timer;
	std::forward_list<command_up> set_up_commands;

	{
		Entities_builder builder;
		builder.save_config(); // - билдер сохраняет конфиг

		fields = builder.build_fields();
		particles = builder.build_particles();
	
	}

	timer.set_up();

	//	if (!set_up_commands_.empty()) {
	//		for (auto& command : set_up_commands_) {
	//			command->execute();
	//		}
	//	}
	
	for (int t = 0; t < TIME; ++t) {	

		#if there_are_particles
		for (auto& sort : particles_) {
			 sort.diagnose(t);
			 sort.propogate();
		}
		#endif
	
		fields_.diagnose(t);
		fields_.propogate();			
		
		timer.tick(t);
	}
			
	auto end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end - start;
	cout << "\n\n\truntime:\t" << elapsed.count() << "s\n" << endl;
	
	return 0;
}