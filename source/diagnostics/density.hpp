#ifndef DIAGNOSTICS_DENSITY_HPP
#define DIAGNOSTICS_DENSITY_HPP

//#################################################################################################

#include "diagnostics.hpp"

#include <string>
#include <vector>

#include "../particles/particle/particle_parameters.hpp"
#include "../particles/particle/point.hpp"


class density : public Particles_diagnostic {
public:
	density(std::string directory_path,
		int begin_x, int begin_y, int end_x, int end_y);

	void save_parameters(std::string directory_path) override;
	void diagnose(const Particle_parameters&, const std::vector<Point>&, int t) override;

private:
	void collect(const Particle_parameters& sort, const std::vector<Point>& points);
	void clear();
	
	std::vector<double> dens_;

	int begin_x_;
	int begin_y_;
	int end_x_;
	int end_y_;
};

//#################################################################################################

#endif // DIAGNOSTICS_DENSITY_HPP
