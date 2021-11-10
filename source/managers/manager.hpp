#include <map>
#include <list>
#include <string>
#include <memory>

#include "../fields/fields.hpp"
#include "../particles/particles.hpp"
#include "../command/command.hpp"
#include "../constants.h"


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

	// Команды
	using Command_up = std::unique_ptr<Command>;
	std::list<Command_up> settings_before_main_cycle;
	std::list<Command_up> each_step_presets;
};