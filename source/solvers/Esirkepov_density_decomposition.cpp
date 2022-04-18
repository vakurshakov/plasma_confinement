#include "Esirkepov_density_decomposition.hpp"

#include <omp.h>
#include <cmath>
#include <functional>

#include "../particles/particle/particle.hpp"
#include "../vectors/vector3_field.hpp"
#include "../constants.h"


void Esirkepov_density_decomposition::process(const Particle& particle, const vector2& r0)
{	
	const int nearest_edge_to_rx = int(round(particle.get_point().x() / dx));
	const int nearest_edge_to_ry = int(round(particle.get_point().y() / dy));

	p_v3f temp_J(2 * charge_cloud_ + 1, 2 * charge_cloud_ + 1);

	decompose_x(particle, r0, nearest_edge_to_rx, nearest_edge_to_ry, temp_J);
	decompose_y(particle, r0, nearest_edge_to_rx, nearest_edge_to_ry, temp_J);
	// decompose_z(point, r0, nearest_edge_to_rx, nearest_edge_to_ry, temp_J);
}


void Esirkepov_density_decomposition::decompose_x(const Particle& particle, const vector2& r0, int ne_x, int ne_y, p_v3f& temp_J)
{
	const double n = particle.n();
	const double q = particle.q();

	const vector2& r = particle.get_point().r();

	long int node_x, node_y;
	node_x = ne_x - charge_cloud_;

	for (int x = -charge_cloud_, y = -charge_cloud_; y <= +charge_cloud_; ++y) {
		node_y = ne_y + y;
		temp_J.x(y,x) = - q * n/Np_ * 0.5 * dx / dt *
				(shape_at_(r.x() - node_x * dx, dx) - shape_at_(r0.x() - node_x * dx, dx)) *
				(shape_at_(r.y() - node_y * dy, dy) + shape_at_(r0.y() - node_y * dy, dy));

		#pragma omp atomic
		J_.x(node_y,node_x) += temp_J.x(y,x);
	}
	
	
	for (int y = -charge_cloud_; y <= +charge_cloud_; ++y) {
		node_y = ne_y + y;
		
		for (int x = -charge_cloud_ + 1; x <= +charge_cloud_; ++x) {
			node_x = ne_x + x;

			// Here is only "=" sign (not "+="!).
			temp_J.x(y,x) = temp_J.x(y,x-1) - q * n/Np_ * 0.5 * dx / dt *
					(shape_at_(r.x() - node_x * dx, dx) - shape_at_(r0.x() - node_x * dx, dx)) *
					(shape_at_(r.y() - node_y * dy, dy) + shape_at_(r0.y() - node_y * dy, dy));

			#pragma omp atomic
			J_.x(node_y, node_x) += temp_J.x(y,x);

		}
	}
}


void Esirkepov_density_decomposition::decompose_y(const Particle& particle, const vector2& r0, int ne_x, int ne_y, p_v3f& temp_J)
{
	const double n = particle.n();
	const double q = particle.q();

	const vector2& r = particle.get_point().r();

	long int node_x, node_y;
	node_y = ne_y - charge_cloud_;
		
	for (int y = -charge_cloud_, x = -charge_cloud_; x <= +charge_cloud_; ++x) {
		node_x = ne_x + x;

		temp_J.y(y,x) = - q * n/Np_ * 0.5 * dy / dt *
				(shape_at_(r.x() - node_x * dx, dx) + shape_at_(r0.x() - node_x * dx, dx)) *
				(shape_at_(r.y() - node_y * dy, dy) - shape_at_(r0.y() - node_y * dy, dy));
	
		#pragma omp atomic
		J_.y(node_y, node_x) += temp_J.y(y,x);
	}

	for (int y = -charge_cloud_ + 1; y <= +charge_cloud_; ++y) {
		node_y = ne_y + y;

		for (int x = -charge_cloud_; x <= +charge_cloud_; ++x) {
			node_x = ne_x + x;

			// Here is only "=" sign (not "+="!).
			temp_J.y(y,x) = temp_J.y(y-1,x) - q * n/Np_ * 0.5 * dy / dt *
					(shape_at_(r.x() - node_x * dx, dx) + shape_at_(r0.x() - node_x * dx, dx)) *
					(shape_at_(r.y() - node_y * dy, dy) - shape_at_(r0.y() - node_y * dy, dy));
		
			#pragma omp atomic
			J_.y(node_y, node_x) += temp_J.y(y,x);
		}
	}
}
	

void Esirkepov_density_decomposition::decompose_z(const Particle& particle, const vector2& r0, int ne_x, int ne_y, p_v3f& temp_J)
{
	const double m = particle.m();
	const double n = particle.n();
	const double q = particle.q();

	const vector2& r = particle.get_point().r();
	const vector3& p = particle.get_point().p();

	const double gamma = sqrt(1. + p.dot(p)/( m * m));
	const double vz = p.z() / gamma;

	long int node_x, node_y;

	for (long int y = -charge_cloud_; y <= +charge_cloud_; ++y) {
		node_y = ne_y + y;

		for (long int x = -charge_cloud_; x <= +charge_cloud_; ++x) {
			node_x = ne_x + x;

			#pragma omp atomic
			J_.z(node_y, node_x) += + q * n/Np_ * vz / 3. *
				(( shape_at_(r.x() - node_x * dx, dx) + 0.5*shape_at_(r0.x() - node_x * dx, dx)) *
					shape_at_(r.y() - node_y * dy, dy) +
				(0.5*shape_at_(r.x() - node_x * dx, dx) + shape_at_(r0.x() - node_x * dx, dx)) * 
					 shape_at_(r0.y() - node_y * dy, dy));
		}
	}
}