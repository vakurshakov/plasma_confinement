#ifndef DIAGNOSTCS_ENERGY_HPP
#define DIAGNOSTCS_ENERGY_HPP

//#################################################################################################

#include "diagnostics.hpp"

#include <string>
#include <vector>

#include "../particles/particle/concrete/particle_interface.hpp"
#include "../particles/particle/parameters/global_parameters.hpp"
#include "../vectors/vector3_field.hpp"


class fields_energy : public Fields_diagnostic {
public:
	fields_energy(std::string directory_path);

	void save_parameters(std::string directory_path);


	using v3f = vector3_field;
	void diagnose(const v3f& E, const v3f& B, const v3f& j, int t) override;
	
private:
	double W = 0;
};


class particles_energy : public Particles_diagnostic {
public:
	particles_energy(std::string directory_path);
	
	void save_parameters(std::string directory_path);

	using particle_up = std::unique_ptr<IParticle>;
	void diagnose(const gParameters&, const std::vector<particle_up>&, int t) override;
	
private:
	double W = 0;
};

//#################################################################################################

#endif // DIAGNOSTCS_ENERGY_HPP