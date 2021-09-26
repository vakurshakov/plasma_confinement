#ifndef PARTICLES_BUILDER_HPP
#define PARTICLES_BUILDER_HPP

//#################################################################################################

#include "particles.hpp"

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <forward_list>

#include "../command/command.hpp"
#include "../fields/fields.hpp"
#include "../solvers/Boris_pusher.hpp"


using std::string, std::vector;
using diagnostic_up = std::unique_ptr<Diagnostic>;


class Particles_builder {
public:
	Particles_builder(Fields* const fields) : fields_(fields) {};
		
	std::forward_list<Particles> build();

private:
	Fields* const fields_;
	
	Particle_parameters config_parameters(const vector<string> parameters);
	vector<Point> load_particles(const vector<string> distribution);
	
	auto set_pusher(const vector<string> description);
	auto particle_push_commands(const vector<string> description,
		Boris_pusher* const, const Particle_parameters&);
	
	auto x_boundary();
	auto y_boundary();

	// Возвращает список диагностик для частиц
	std::forward_list<diagnostic_up> diagnostics_list(string particle_name,
		vector<string> description);
};


//#################################################################################################

#endif //PARTICLES_BUILDER_HPP