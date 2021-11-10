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

#include "../command/command.hpp"
#include "../fields/fields.hpp"
#include "../solvers/Boris_pusher.hpp"


using std::string, std::vector;


class Particles_builder {
public:
	Particles_builder() {};
	
	using Particles_up = std::unique_ptr<Particles>;
	using Command_up = std::unique_ptr<Command>;

	std::map<string, Particles_up> build(Fields& fields,
		std::list<Command_up>& settings_before_main_cycle);

private:
	
	Particle_parameters config_parameters(const vector<string> parameters);
	void load_particles(
		Particles* const particles,
		const vector<string> distribution,
		const Particle_parameters& parameters,
		std::list<Command_up>& settings_before_main_cycle);
	
	auto choose_pusher(
		const vector<string> description,
		const Particle_parameters&, Fields& fields);
	auto choose_interpolation(
		const vector<string> description,
		const Particle_parameters&, Fields& fields);
	auto choose_decomposition(
		const vector<string> description,
		const Particle_parameters&, Fields& fields);

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