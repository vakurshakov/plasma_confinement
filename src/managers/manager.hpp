#include <memory>
#include <forward_list>

#include "./command/command.hpp"
#include "./managers/fields.hpp"
#include "./managers/particles.hpp"
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

	// Комманды
	std::forward_list<command_up> set_up_commands;
}