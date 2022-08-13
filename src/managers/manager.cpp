#include "manager.hpp"

#include "time_manager.hpp"
#include "../command/command.hpp"
#include "../command/cmd_add_Bz0.hpp"
#include "../command/ionize_particles.hpp"
#include "../command/magnetic_field_half_step.hpp"
#include "../diagnostics/chosen_particles.hpp"
#include "../fields/fields_builder.hpp"
#include "../particles/particles_builder.hpp"
#include "../particles/particles_load.hpp"


void Manager::initializes()
{
	std::list<Command_up> presets;

	Fields_builder fields_builder;
	fields_ = std::move(fields_builder.build());
	
	each_step_presets.push_front(std::make_unique<Magnetic_field_half_step>(&fields_));

#if there_are_fields
	#if there_are_Bz0
		presets.push_back(std::make_unique<Add_Bz0>(&fields_, Bz0));
	#endif
#endif
	
	Particles_builder particles_builder;
	list_of_particles_ = particles_builder.build(fields_, presets);		

// TODO: fabric on each_step_presets
// TODO: variable set_time_distribution
#if there_are_ions
	std::cout << "\tSetting each step presets..."
		<< "\n\t\tIonization "; 

	Particles* const ionized = list_of_particles_["beam_ions"].get();
	Particles* const lost = list_of_particles_["beam_electrons"].get();

	int Np_il = ionized->get_parameters().Np();
	int total_number_of_particles = get_number_of_particles_on_circle_segment(Np_il);
	
	auto time_distribution = set_time_distribution(TINJ, total_number_of_particles);

	each_step_presets.push_front(std::make_unique<Ionize_particles>(
		ionized, lost,
		time_distribution,
		set_point_on_circle_segment,
		get_cosine_probability,
		load_annular_impulse
	));
	
	std::cout << "\n\t\tcheck : (&ionized=" << ionized << ", "
			  << "&lost=" << lost << ")";

	std::cout << "\n\t\tdone\n"
		<< "\tdone!" << std::endl;
#endif

 	for (const auto& command : presets)
		command->execute(0);
	presets.clear();

#if there_are_electrons && there_are_plasma_ions
	Particles* const plasma_electrons = list_of_particles_["plasma_electrons"].get();
	Particles* const plasma_ions = list_of_particles_["plasma_ions"].get();
	
	current_diagnostics.push_back(Single_field_diagnostic(dir_name + "/fields/whole_field/Jpe_x", plasma_electrons->J, Axis::X, 1000, 2000, 1000, 2000));
	current_diagnostics.push_back(Single_field_diagnostic(dir_name + "/fields/whole_field/Jpe_y", plasma_electrons->J, Axis::Y, 1000, 2000, 1000, 2000));
	current_diagnostics.push_back(Single_field_diagnostic(dir_name + "/fields/whole_field/Jpi_x", plasma_ions->J, Axis::X, 1000, 2000, 1000, 2000));
	current_diagnostics.push_back(Single_field_diagnostic(dir_name + "/fields/whole_field/Jpi_y", plasma_ions->J, Axis::Y, 1000, 2000, 1000, 2000));
#endif

#if there_are_ions
	current_diagnostics.push_back(Single_field_diagnostic(dir_name + "/fields/whole_field/Jbi_x", ionized->J, Axis::X, 1000, 2000, 1000, 2000));
	current_diagnostics.push_back(Single_field_diagnostic(dir_name + "/fields/whole_field/Jbi_y", ionized->J, Axis::Y, 1000, 2000, 1000, 2000));
	current_diagnostics.push_back(Single_field_diagnostic(dir_name + "/fields/whole_field/Jbe_x", lost->J, Axis::X, 1000, 2000, 1000, 2000));
	current_diagnostics.push_back(Single_field_diagnostic(dir_name + "/fields/whole_field/Jbe_y", lost->J, Axis::Y, 1000, 2000, 1000, 2000));
#endif
}


void Manager::calculates()
{
	std::cout << "\n\tProcessing main cycle..." << std::endl;
	Timer timer;
	timer.set_up();
	
	for (int t = 0; t < TIME; ++t)
	{	
		// Сlearing the list of unnecessary elements.
		each_step_presets.remove_if(
			[t](const Command_up& command){
			return command->needs_to_be_removed(t);
		});

		// Executing the rest of the commands.
		for (const auto& command : each_step_presets)
		{
			command->execute(t);
		}

	
		#if there_are_particles
		for (auto& [_, particles] : list_of_particles_) {
			particles->diagnose(t);
			particles->push();
		}
		#endif

		#pragma omp parallel for shared(current_diagnostics)
		for(const auto& d : current_diagnostics)
		{
			d.diagnose(t);
		}

		this->aggregate_sources();

		#if there_are_fields
		fields_.diagnose(t);
		fields_.propogate();
		#endif

		timer.tick(t);
 	}

	timer.elapsed();
}

void Manager::aggregate_sources()
{
	v3f& J = fields_.J();

	#pragma omp parallel for
	for (int ny = 0; ny < SIZE_Y; ++ny) {
	for (int nx = 0; nx < SIZE_X; ++nx) {
	
		for(const auto& [_, sort]: list_of_particles_)
		{
			v3f& Js = sort->J;

			J.x(ny,nx) += Js.x(ny,nx);
			J.y(ny,nx) += Js.y(ny,nx);
			J.z(ny,nx) += Js.z(ny,nx);

			Js.x(ny,nx) = 0.;
			Js.y(ny,nx) = 0.;
			Js.z(ny,nx) = 0.;
		}
	}}
}
