#ifndef DIAGNOSTICS_CHOSEN_PARTICLES_HPP
#define DIAGNOSTICS_CHOSEN_PARTICLES_HPP

//#################################################################################################

#include "diagnostics.hpp"

#include <string>
#include <vector>

#include "../particles/particle/particle.hpp"

std::vector<int> choose_indexes();


class chosen_particles : public Particles_diagnostic {
public:
	chosen_particles(std::string directory_path,
        std::vector<int> indexes_of_chosen_particles);

	void save_parameters(std::string directory_path) override;

	void diagnose(const Parameters&, const std::vector<Particle>&, int t) override;

private:
	std::vector<int> indexes_of_chosen_particles_;
};

//#################################################################################################

#endif // DIAGNOSTICS_CHOSEN_PARTICLES_HPP
