#ifndef DIAGNOSTICS_CHOSEN_PARTICLES_HPP
#define DIAGNOSTICS_CHOSEN_PARTICLES_HPP

//#################################################################################################

#include "diagnostics.hpp"

#include <string>
#include <vector>

#include "../particles/particle/particle_parameters.hpp"
#include "../particles/particle/point.hpp"


std::vector<int> choose_indexes();


class chosen_particles : public Particles_diagnostic {
public:
	chosen_particles(std::string directory_path,
        std::vector<int> indexes_of_chosen_particles);

	void save_parameters(std::string directory_path) override;
	void diagnose(const Particle_parameters&, const std::vector<Point>&, int t) override;

private:
	std::vector<int> indexes_of_chosen_particles_;
};

//#################################################################################################

#endif // DIAGNOSTICS_CHOSEN_PARTICLES_HPP
