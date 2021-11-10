#ifndef COMMAND_CMD_IONIZE_PARTICLES_HPP
#define COMMAND_CMD_IONIZE_PARTICLES_HPP

//#######################################################################################

#include "command.hpp"

#include <memory>

#include "../particles/particles.hpp"
#include "../particles/add_ionization.hpp"

using Ionization_up = std::unique_ptr<Ionization>;


class Ionize_particles : public Command {
public:
	Ionize_particles(
		Ionization_up _ionization,
		Particles* const _ionized, Particles* const _lost) 
			:	ionization(std::move(_ionization)),
				ionized(_ionized), lost(_lost) {};

	void execute(int t) const override {
		ionization->process(ionized, lost, t);
	};

	bool this_is_time_to_remove(int t) {
		if ( t >= TINJ)
			return true;
	}

private:
	const Ionization_up ionization;
	Particles* const ionized;
	Particles* const lost;
};

//#######################################################################################

#endif // COMMAND_CMD_IONIZE_PARTICLES_HPP