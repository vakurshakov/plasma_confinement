#ifndef DIAGNOSTICS_CHOSEN_PARTICLES_HPP
#define DIAGNOSTICS_CHOSEN_PARTICLES_HPP

//#################################################################################################

#include "diagnostics.hpp"

#include <string>
#include <vector>

#include "../particles/particle/concrete/particle_interface.hpp"
#include "../particles/particle/parameters/global_parameters.hpp"


std::vector<int> choose_indexes();


class chosen_particles : public Particles_diagnostic {
public:
	chosen_particles(std::string directory_path,
        std::vector<int> indexes_of_chosen_particles);

	void save_parameters(std::string directory_path) override;

	using particle_up = std::unique_ptr<IParticle>;
	void diagnose(const gParameters&, const std::vector<particle_up>&, int t) override;

private:
	std::vector<int> indexes_of_chosen_particles_;
};

//#################################################################################################

#endif // DIAGNOSTICS_CHOSEN_PARTICLES_HPP
