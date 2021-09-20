#include "./diagnostics.hpp"

#include <cmath>
#include <string>
#include <vector>

#include "../particle/particle_parameters.hpp"
#include "../particle/point.hpp"


class diagram_vx_on_y : public Diagnostic {
public:
	diagram_vx_on_y(std::string path, 
		double vmin, double vmax, double dv, int nYmin, int nYmax)
	: Diagnostic(path, "diagram_vx_on_y"),
	vmin_(vmin), vmax_(vmax), dv_(dv), nYmin_(nYmin), nYmax_(nYmax) {

		nVmin_ = int(roundf(vmin_/dv_));
		nVmax_ = int(roundf(vmax_/dv_));
		
		//save_parameters();
	};

	void save_parameters(std::string directory_path) override;
	void diagnose(const Particle_parameters&, const std::vector<Point>&, int t) override;

	friend void collect_diagram_vx_on_y(diagram_vx_on_y& diag,
		const Particle_parameters& sort, const std::vector<Point>& points);
	friend void clear_diagram_vx_on_y(diagram_vx_on_y& diag);

protected:
	std::vector<double> data_;

	double vmin_;
	double vmax_;
	double dv_;
	
	int nYmin_;
	int nYmax_;
	int nVmin_;
	int nVmax_;
};
