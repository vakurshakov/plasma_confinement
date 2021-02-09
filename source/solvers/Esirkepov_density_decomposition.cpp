#ifndef VECTOR_CLASSES_H
#define VECTOR_CLASSES_H
	#include "../vectors/vector_classes.hpp"
#endif

#ifndef VECTOR3_FIELD_H
#define VECTOR3_FIELD_H
	#include "../vectors/vector3_field.hpp"
#endif

#ifndef PARTICLES_H
#define PARTICLES_H
	#include "../particles/particles.hpp"
#endif

#include <cmath>
#include <omp.h>

/*

	NOTE: try to add 1/2 to calculate fields at shifted points

*/

void Esirkepov_density_decomposition(const sort_of_particles& SORT,
									 const particle& PARTICLE,
									 const vector2& r0,
									 vector3_field& j,
									 double dt, double dx, double dy)
{
	double gamma = sqrt(1 + PARTICLE.p().dot(PARTICLE.p())/(SORT.m()*SORT.m()));
	double vz = PARTICLE.p().z()/gamma;
	vector2 r = PARTICLE.r();
	double q  = SORT.q();
	double Np = SORT.Np();
	int charge_cloud = SORT.charge_cloud();
	double (*shape_at)(double, double) = SORT.form_factor();

	periodic_vector3_field temp_j(2*charge_cloud+1, 2*charge_cloud+1); 

	size_t nearest_edge_to_rx = size_t(roundf(r.x()/dx));
	size_t nearest_edge_to_ry = size_t(roundf(r.y()/dy));

	{	// collecting jx(y,x) -------------------------------------------------------------------------
		long int node_x, node_y;
		node_x = nearest_edge_to_rx - charge_cloud;
		
		for (long int x = -charge_cloud, y = -charge_cloud; y <= +charge_cloud; ++y) {
			node_y = nearest_edge_to_ry + y;
	
			temp_j.x(y,x) = - q/Np*0.5*dx/dt*(shape_at(r.x() - node_x*dx, dx) - shape_at(r0.x() - node_x*dx, dx))*
									 		 (shape_at(r.y() - node_y*dy, dy) + shape_at(r0.y() - node_y*dy, dy));
	
			#pragma omp atomic
			j.x(node_y,node_x) += temp_j.x(y,x);
		}
	
	
		for (long int y = -charge_cloud; y <= +charge_cloud; ++y) {
			node_y = nearest_edge_to_ry + y;
			
			for (long int x = -charge_cloud+1; x <= +charge_cloud; ++x) {
				node_x = nearest_edge_to_rx + x;
	
				temp_j.x(y,x) = temp_j.x(y,x-1) - q/Np*0.5*dx/dt*(shape_at(r.x() - node_x*dx, dx) - shape_at(r0.x() - node_x*dx, dx))*
																 (shape_at(r.y() - node_y*dy, dy) + shape_at(r0.y() - node_y*dy, dy));
	
				#pragma omp atomic
				j.x(node_y, node_x) += temp_j.x(y,x);
	
			}
		}
	}

	{	// collecting jy(y,x) -------------------------------------------------------------------------
		long int node_x, node_y;
		node_y = nearest_edge_to_ry - charge_cloud;
		
		for (long int y = -charge_cloud, x = -charge_cloud; x <= +charge_cloud; ++x) {
			node_x = nearest_edge_to_rx + x;
	
			temp_j.y(y,x) = - q/Np*0.5*dy/dt*(shape_at(r.x() - node_x*dx, dx) + shape_at(r0.x() - node_x*dx, dx))*
											 (shape_at(r.y() - node_y*dy, dy) - shape_at(r0.y() - node_y*dy, dy));
		
			#pragma omp atomic
			j.y(node_y, node_x) += temp_j.y(y,x);
		}
	
		for (long int y = -charge_cloud+1; y <= +charge_cloud; ++y) {
			node_y = nearest_edge_to_ry + y;
	
			for (long int x = -charge_cloud; x <= +charge_cloud; ++x) {
				node_x = nearest_edge_to_rx + x;
	
				temp_j.y(y,x) += temp_j.y(y-1,x) - q/Np*0.5*dy/dt*(shape_at(r.x() - node_x*dx, dx) + shape_at(r0.x() - node_x*dx, dx))*
																  (shape_at(r.y() - node_y*dy, dy) - shape_at(r0.y() - node_y*dy, dy));
			
				#pragma omp atomic
				j.y(node_y, node_x) += temp_j.y(y,x);
			}
		}
	}

	/*
	{	// collecting jz(y,x) -------------------------------------------------------------------------
		for (long int x = nearest_edge_to_rx - charge_cloud; x <= nearest_edge_to_rx + charge_cloud; ++x) {
		for (long int y = nearest_edge_to_ry - charge_cloud; y <= nearest_edge_to_ry + charge_cloud; ++y) {
	
			#pragma omp atomic
			j.z(y,x) += - q/Np*vz/3.*((	shape_at(r.x) - (x*dx, dx) + 0.5*shape_at(r0.x) - (x*dx, dx))*shape_at(r.y(  - (y*dy, dy)+
								   (0.5*shape_at(r.x) - (x*dx, dx) +	 shape_at(r0.x) - (x*dx, dx))*shape_at(r0.y) - (y*dy, dy));
			}
		}
	}
	*/
}