#include "./particles.hpp"

Particles::Particles(
	Particle_parameters parameters,
	std::vector<Point> points,
	std::function<void(const Particle_parameters&, Point&,
				const v3f& E, const v3f& B)> particle_push,
	std::function<void(const Particle_parameters&, const Point&,
				const vector2& r0, v3f& j)> density_decomposition,
	std::function<void(Point&, double)> x_boundary,
	std::function<void(Point&, double)> y_boundary,
	std::forward_list<diagnostic_up>& diagnostics )
{
	parameters_ = parameters;
	points_.swap(points);
	particle_push_ = particle_push;
	density_decomposition_ = density_decomposition;
	x_boundary_ = x_boundary;
	y_boundary_ = y_boundary;
	diagnostics_.swap(diagnostics);
}

void Particles::push_particles()
{
	#pragma omp parallel for num_threads(THREAD_NUM)
	for (decltype(particles.size()) i = 0; i < particles.size(); ++i) {
	
		vector2 r0 = sort.element(i).r();
		
		sort.particle_push(i, fields_.E(), fields_.B());
		
		sort.density_decomposition(i, r0, fields_.j());
		
		sort.boundaries_processing(i, SIZE_X*dx, SIZE_Y*dy);
	}
}

void Particles::particle_push(int i, const v3f& E, const v3f& B)
{
	particle_push_(parameters_, points_[i], E, B);
} 


void Particles::density_decomposition(int i, const vector2& r0, v3f& j)
{
	density_decomposition_(parameters_, points_[i], r0, j);
}


void Particles::boundaries_processing(int i, double size_x, double size_y)
{
	x_boundary_(points_[i], size_x);
	y_boundary_(points_[i], size_y);
}


void Particles::diagnose(int t)
{
	if (!diagnostics_.empty()) {
		for (auto& diagnostic : diagnostics_) {
			diagnostic->diagnose(parameters_, points_, t);
		} 
	}
}
