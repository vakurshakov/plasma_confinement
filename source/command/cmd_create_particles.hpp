#ifndef COMMAND_CMD_CREATE_PARTICLES_HPP
#define COMMAND_CMD_CREATE_PARTICLES_HPP

//#######################################################################################

#include "command.hpp"

#include <functional>

#include "../particles/particles.hpp"


class Create_particles : public Command {
public:
	Create_particles() {};

	void execute() const override {};

private:
	
};

//#######################################################################################

#endif // COMMAND_CMD_CREATE_PARTICLES_HPP
