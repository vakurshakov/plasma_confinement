#ifndef PARTICLES_BUILDER_HPP
#define PARTICLES_BUILDER_HPP

//#################################################################################################

#include "particles.hpp"

#include <map>
#include <list>
#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "./particle/parameters.hpp"
#include "../command/command.hpp"
#include "../fields/fields.hpp"

using std::string, std::vector;


/**
 * Builder for particles.
 * 
 * @brief This class builds all types of particles
 * 		prediscribed in `constants.h`
 * 
 * @todo Builder for a single Particles object (?)
 */
class Particles_builder {
public:
	// Default constructor.
	Particles_builder() = default;
	
	using Particles_up = std::unique_ptr<Particles>;
	using Command_up = std::unique_ptr<Command>;

	/**
	 * @brief Constructs a list of particles used in a
	 * 		computational experiment.
	 * 
	 * @param fields Reference to an outer Fields object,
	 * 		to set particle-field and field-particle interaction.
	 * @param settings_before_main_cycle Reference to a list of
	 * 		commands to insert particle initialization there. 
	 * 
	 * @return List of particles with names, according to its
	 * 		desctription in `constants.h`.
	 */
	std::map<string, Particles_up> build(Fields& fields,
		std::list<Command_up>& settings_before_main_cycle);

private:
	
	auto config_parameters(const vector<string> parameters);
	
	void load_particles(
		Particles* const particles,
		const vector<string> distribution,
		const Parameters& parameters,
		std::list<Command_up>& settings_before_main_cycle);
	
	auto choose_pusher(
		const vector<string> description,
		const Parameters&, Fields& fields);
	auto choose_interpolation(
		const vector<string> description,
		const Parameters&, Fields& fields);
	auto choose_decomposition(
		const vector<string> description,
		const Parameters&, Fields& fields);

	auto x_boundary();
	auto y_boundary();

	// Возвращает список диагностик для частиц
	using diagnostic_up = std::unique_ptr<Particles_diagnostic>;
	
	vector<diagnostic_up> diagnostics_list(
		string particle_name,
		vector<string> description);
};


//#################################################################################################

#endif //PARTICLES_BUILDER_HPP