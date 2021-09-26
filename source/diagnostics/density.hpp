#ifndef DIAGNOSTICS_DENSITY_HPP
#define DIAGNOSTICS_DENSITY_HPP

//#################################################################################################

#include "diagnostics.hpp"

#include <string>
#include <vector>

#include "../particles/particle/particle_parameters.hpp"
#include "../particles/particle/point.hpp"


class density : public Diagnostic {
public:
	density(std::string directory_path);

	void save_parameters(std::string directory_path) override;
	void diagnose(const Particle_parameters&, const std::vector<Point>&, int t) override;

private:
	std::vector<double> dens_;

	void collect(const Particle_parameters& sort, const std::vector<Point>& points);
	void clear();
};

//#################################################################################################

#endif // DIAGNOSTICS_DENSITY_HPP
