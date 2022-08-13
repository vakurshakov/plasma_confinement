#ifndef DIAGNOSTCS_ENERGY_HPP
#define DIAGNOSTCS_ENERGY_HPP

//#################################################################################################

#include "diagnostics.hpp"

#include "src/pch.h"
#include "../particles/particle/particle.hpp"
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

	void diagnose(const Parameters&, const std::vector<Particle>&, int t) override;
	
private:
	double W = 0;
};

//#################################################################################################

#endif // DIAGNOSTCS_ENERGY_HPP