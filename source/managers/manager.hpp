#include <map>
#include <list>
#include <string>
#include <memory>

#include "../fields/fields.hpp"
#include "../particles/particles.hpp"
#include "../command/command.hpp"
#include "../constants.h"
#include "../diagnostics/single_field.hpp"


class Manager {
public:
	Manager() = default;

	void initializes();
	void calculates();

private:
	// Необходимые для работы cущности
	Fields fields_;

	using particles_up = std::unique_ptr<Particles>;
	std::map<std::string, particles_up> list_of_particles_;

	void aggregate_sources();

	std::vector<Single_field_diagnostic> current_diagnostics;

	// Команды
	using Command_up = std::unique_ptr<Command>;
	std::list<Command_up> each_step_presets;
};