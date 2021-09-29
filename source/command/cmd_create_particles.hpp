#ifndef COMMAND_CMD_CREATE_PARTICLES_HPP
#define COMMAND_CMD_CREATE_PARTICLES_HPP

//#######################################################################################

#include "command.hpp"

#include <memory>
#include <functional>

#include "../particles/particles.hpp"
#include "../particles/add_ionization.hpp"

using Ionization_up = std::unique_ptr<Ionization>;


class Create_particles : public Command {
public:
	Create_particles(
		Ionization_up& _ionization,
		Particles* const _ionized, Particles* const _lost) 
			:	ionization(std::move(_ionization)),
				ionized(_ionized), lost(_lost) {};

	void execute() const override {
		ionization->process(ionized, lost);	
	};

private:
	const Ionization_up ionization;
	Particles* const ionized;
	Particles* const lost;
};

//#######################################################################################

#endif // COMMAND_CMD_CREATE_PARTICLES_HPP
