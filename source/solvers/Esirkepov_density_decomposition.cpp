#include "Esirkepov_density_decomposition.hpp"

#include <omp.h>
#include <cmath>
#include <functional>

#include "../particles/particle/particle_parameters.hpp"
#include "../particles/particle/point.hpp"
#include "../vectors/vector3_field.hpp"
#include "../constants.h"


/*
class temp_Field_component {
public:
	temp_Field_component(int size) : size_(size) {
		component_.reserve(size*size);
	}

	double& operator()(int y, int x) 		{ return component_[2*size_ + 1]; }
	double  operator()(int y, int x) const 	{ return component_[2*size_ + 1]; }


private:
	std::vector<double> component_;
	const int size_; 
};
*/


void Esirkepov_density_decomposition::process(const Point& point, const vector2& r0){
	const vector2& r = point.r(); 
	const int nearest_edge_to_rx = int(roundf(r.x/dx));
	const int nearest_edge_to_ry = int(roundf(r.y/dy));
	p_v3f temp_J(2*charge_cloud_+1, 2*charge_cloud_+1);

	decompose_x(r, r0, nearest_edge_to_rx, nearest_edge_to_ry, temp_J);
	decompose_y(r, r0, nearest_edge_to_rx, nearest_edge_to_ry, temp_J);
	// decompose_z(point, r0, nearest_edge_to_rx, nearest_edge_to_ry, temp_J);
}

void Esirkepov_density_decomposition::decompose_x(const vector2& r, const vector2& r0, int ne_x, int ne_y, p_v3f& temp_J)
{	
	// collecting jx(y,x) -------------------------------------------------------------------------	
	long int node_x, node_y;
	node_x = ne_x - charge_cloud_;

	for (int x = -charge_cloud_, y = -charge_cloud_; y <= +charge_cloud_; ++y) {
		node_y = ne_y + y;
		temp_J.x(y,x) = - q_*n_/Np_*0.5*dx/dt*
				(shape_at_(r.x - node_x*dx, dx) - shape_at_(r0.x - node_x*dx, dx))*
				(shape_at_(r.y - node_y*dy, dy) + shape_at_(r0.y - node_y*dy, dy));

		#pragma omp atomic
		J_.x(node_y,node_x) += temp_J.x(y,x);
	}
	
	
	for (int y = -charge_cloud_; y <= +charge_cloud_; ++y) {
		node_y = ne_y + y;
		
		for (int x = -charge_cloud_+1; x <= +charge_cloud_; ++x) {
			node_x = ne_x + x;

			// here is only "=" sign (not "+="!)
			temp_J.x(y,x) = temp_J.x(y,x-1) - q_*n_/Np_*0.5*dx/dt*
					(shape_at_(r.x - node_x*dx, dx) - shape_at_(r0.x - node_x*dx, dx))*
					(shape_at_(r.y - node_y*dy, dy) + shape_at_(r0.y - node_y*dy, dy));

			#pragma omp atomic
			J_.x(node_y, node_x) += temp_J.x(y,x);

		}
	}
}
	
void Esirkepov_density_decomposition::decompose_y(const vector2& r, const vector2& r0, int ne_x, int ne_y, p_v3f& temp_J)
{	// collecting jy(y,x) -------------------------------------------------------------------------
	long int node_x, node_y;
	node_y = ne_y - charge_cloud_;
		
	for (int y = -charge_cloud_, x = -charge_cloud_; x <= +charge_cloud_; ++x) {
		node_x = ne_x + x;

		temp_J.y(y,x) = - q_*n_/Np_*0.5*dy/dt*
				(shape_at_(r.x - node_x*dx, dx) + shape_at_(r0.x - node_x*dx, dx))*
				(shape_at_(r.y - node_y*dy, dy) - shape_at_(r0.y - node_y*dy, dy));
	
		#pragma omp atomic
		J_.y(node_y, node_x) += temp_J.y(y,x);
	}

	for (int y = -charge_cloud_+1; y <= +charge_cloud_; ++y) {
		node_y = ne_y + y;

		for (int x = -charge_cloud_; x <= +charge_cloud_; ++x) {
			node_x = ne_x + x;

			// here is only "=" sign (not "+="!)
			temp_J.y(y,x) = temp_J.y(y-1,x) - q_*n_/Np_*0.5*dy/dt*
					(shape_at_(r.x - node_x*dx, dx) + shape_at_(r0.x - node_x*dx, dx))*
					(shape_at_(r.y - node_y*dy, dy) - shape_at_(r0.y - node_y*dy, dy));
		
			#pragma omp atomic
			J_.y(node_y, node_x) += temp_J.y(y,x);
		}
	}
}
	
void Esirkepov_density_decomposition::decompose_z(const Point& point, const vector2& r0, int ne_x, int ne_y, p_v3f& temp_J)
{	// collecting jz(y,x) -------------------------------------------------------------------------
	long int node_x, node_y;

	const vector2& r = point.r(); 
	const double gamma = sqrt(1 + point.p().dot(point.p())/(m_*m_));
	const double vz = point.p().z/gamma;

	for (long int y = -charge_cloud_; y <= +charge_cloud_; ++y) {
		node_y = ne_y + y;

		for (long int x = -charge_cloud_; x <= +charge_cloud_; ++x) {
			node_x = ne_x + x;

			#pragma omp atomic
			J_.z(node_y, node_x) += + q_*n_/Np_*vz/3.*
				(( shape_at_(r.x - node_x*dx, dx) + 0.5*shape_at_(r0.x - node_x*dx, dx))*shape_at_(r.y  -  node_y*dy, dy)+
				(0.5*shape_at_(r.x - node_x*dx, dx) + shape_at_(r0.x - node_x*dx, dx))*shape_at_(r0.y -  node_y*dy, dy));
		}
	}
}