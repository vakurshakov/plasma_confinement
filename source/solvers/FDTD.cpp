#include "../vectors/vector3_field.hpp"
#include "../constants.h"

#include <omp.h>

void FDTD_2D(vector3_field& E, vector3_field& B, vector3_field& J) 
{	// in 2D-FDTD fields are independent of z, so every %/dz fractions will be zero
	
	#pragma omp parallel shared(E, B, J), num_threads(THREAD_NUM)
	{
	/*
		// Bx(y, x+1/2) at t+1/2 ----------------------------------------------------
		#pragma omp for 
		for (int y = B.begin_y(X); y < B.end_y(X); ++y) {
			for (int x = B.begin_x(X); x < B.end_x(X); ++x) {
					B.x(y,x) -= 0.5*(E.z(y,x) - E.z(y-1,x))*dt/dy;			
			}
		}	

		// By(y+1/2, x) at t+1/2 ----------------------------------------------------
		#pragma omp for
		for (int y = B.begin_y(Y); y < B.end_y(Y); ++y) {
			for (int x = B.begin_x(Y); x < B.end_x(Y); ++x) {
					B.y(y,x) += 0.5*(E.z(y,x) - E.z(y,x-1))*dt/dx;
			}
		}
	*/
	
		// Bz(y, x) at t+1/2 --------------------------------------------------------
		#pragma omp for
		for (int y = B.begin_y(Z); y < B.end_y(Z); ++y) {
			for (int x = B.begin_x(Z); x < B.end_x(Z); ++x) {
					B.z(y,x) -= 0.5*((E.y(y,x+1) - E.y(y,x))/dx - (E.x(y+1,x) - E.x(y,x))/dy)*dt;
			}
		}

		// Ex(y+1/2, x) at t+1 ------------------------------------------------------
		#pragma omp for
		for (int y = E.begin_y(X); y < E.end_y(X); ++y) {
			for (int x = E.begin_x(X); x < E.end_x(X); ++x) {
				E.x(y,x) += -J.x(y,x)*dt + (B.z(y,x) - B.z(y-1,x))*dt/dy;
				J.x(y,x) = 0;
			}	
		}

		// Ey(y, x+1/2) at t+1 ------------------------------------------------------
		#pragma omp for
		for (int y = E.begin_y(Y); y < E.end_y(Y); ++y) { 
			for (int x = E.begin_x(Y); x < E.end_x(Y); ++x) {
				E.y(y,x) += -J.y(y,x)*dt - (B.z(y,x) - B.z(y,x-1))*dt/dx;
				J.y(y,x) = 0;
			}		
		}

	/*
		// Ez(y+1/2,x+1/2) at t+1 ---------------------------------------------------
		#pragma omp for
		for (int y = E.begin_y(Z); y < E.end_y(Z); ++y) {
			for (int x = E.begin_x(Z); x < E.end_x(Z); ++x) {
				E.z(y,x) += -J.z(y,x)*dt + ((B.y(y,x+1) - B.y(y,x))/dx - (B.x(y+1,x) - B.x(y,x))/dy)*dt;
				J.z(y,x) = 0;
			}	
		}

		// Bx(y, x+1/2) at t+1/2 ----------------------------------------------------
		#pragma omp for
		for (int y = B.begin_y(X); y < B.end_y(X); ++y) {
			for (int x = B.begin_x(X); x < B.end_x(X); ++x) {
					B.x(y,x) -= 0.5*(E.z(y,x) - E.z(y-1,x))*dt/dy;			
			}
		}	

		// By(y+1/2, x) at t+1/2 ----------------------------------------------------
		#pragma omp for
		for (int y = B.begin_y(Y); y < B.end_y(Y); ++y) {
			for (int x = B.begin_x(Y); x < B.end_x(Y); ++x) {
					B.y(y,x) += 0.5*(E.z(y,x) - E.z(y,x-1))*dt/dx;
			}
		}
	*/

		// Bz(y, x) at t+1/2 --------------------------------------------------------
		#pragma omp for
		for (int y = B.begin_y(Z); y < B.end_y(Z); ++y) {
			for (int x = B.begin_x(Z); x < B.end_x(Z); ++x) {
					B.z(y,x) -= 0.5*((E.y(y,x+1) - E.y(y,x))/dx - (E.x(y+1,x) - E.x(y,x))/dy)*dt;
			}
		}
	}
}