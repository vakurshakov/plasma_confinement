#include <memory>
#include <forward_list>

#include "./fields.hpp"
#include "./particles.hpp"
#include "../command/command.hpp"
#include "../constants.h"


using fcommand_up = std::unique_ptr<Fields_command>;


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
	std::forward_list<fcommand_up> fields_setting_commands_;
};