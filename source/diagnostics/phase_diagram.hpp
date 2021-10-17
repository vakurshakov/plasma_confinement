#include "diagnostics.hpp"

#include <cmath>
#include <string>
#include <vector>

#include "../particles/particle/particle_parameters.hpp"
#include "../particles/particle/point.hpp"
#include "../constants.h"


class diagram_vx_on_y : public Particles_diagnostic {
public:
	diagram_vx_on_y(std::string path, double dv);

	void save_parameters(std::string directory_path) override;
	void diagnose(const Particle_parameters&, const std::vector<Point>&, int t) override;

private:
	void collect_diagram_vx_on_y(const Particle_parameters&, const std::vector<Point>&);
	void clear_diagram_vx_on_y();

	std::vector<double> data_;

	double v_min_ = -1;
	double v_max_ =  1;
	double dv_	  =  1e-2;

	int ny_max_ = SIZE_Y;
	int ny_min_ = 0;
	int nv_max_;
	int nv_min_;
};
