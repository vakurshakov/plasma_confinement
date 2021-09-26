#include <map>
#include <string>
#include <memory>

#include "../fields/fields.hpp"
#include "../particles/particles.hpp"
#include "../command/command.hpp"
#include "../constants.h"


using command_up = std::unique_ptr<Command>;


class Manager {
public:
	Manager() = default;

	void initializes();
	void calculates();

private:
	// Необходимые для работы cущности
	Fields fields_;
	std::forward_list<Particles> list_of_particles_;

	// Команды
	std::forward_list<command_up> settings_before_main_cycle;
	std::forward_list<command_up> each_step_presets;
};