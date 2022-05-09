#include "FDTD.hpp"

#include <omp.h>

#include "../vectors/vector3_field.hpp"
#include "../constants.h"

//плохо плохо плохо!
// сначала подкручиваем магнитное поле (см. magnetic_field_half_step),
// затем продвигаем электрическое поле.
void FDTD_2D(vector3_field& E, vector3_field& B, vector3_field& J) 
{	
	// in 2D-FDTD fields are independent of z, so every %/dz fractions will be zero
	#pragma omp parallel shared(E, B, J), num_threads(THREAD_NUM)
	{
		// Bx(y, x+1/2) at t+1/2 ----------------------------------------------------
		#pragma omp for 
		for (int ny = B.iy_first(X); ny < B.iy_last(X); ++ny) {
			for (int nx = B.ix_first(X); nx < B.ix_last(X); ++nx) {
				B.x(ny,nx) -= 0.5*(E.z(ny,nx) - E.z(ny-1,nx))*dt/dy;			
			}
		}	

		// By(y+1/2, x) at t+1/2 ----------------------------------------------------
		#pragma omp for
		for (int ny = B.iy_first(Y); ny < B.iy_last(Y); ++ny) {
			for (int nx = B.ix_first(Y); nx < B.ix_last(Y); ++nx) {
				B.y(ny,nx) += 0.5*(E.z(ny,nx) - E.z(ny,nx-1))*dt/dx;
			}
		}
	
		// Bz(y, x) at t+1/2 --------------------------------------------------------
		#pragma omp for
		for (int ny = B.iy_first(Z); ny < B.iy_last(Z); ++ny) {
			for (int nx = B.ix_first(Z); nx < B.ix_last(Z); ++nx) {
				B.z(ny,nx) -= 0.5*((E.y(ny,nx+1) - E.y(ny,nx))/dx 
								 - (E.x(ny+1,nx) - E.x(ny,nx))/dy)*dt;
			}
		}


		// Ex(y+1/2, x) at t+1 ------------------------------------------------------
		#pragma omp for
		for (int ny = E.iy_first(X); ny < E.iy_last(X); ++ny) {
			for (int nx = E.ix_first(X); nx < E.ix_last(X); ++nx) {
				E.x(ny,nx) += -(J.x(ny,nx))*dt + (B.z(ny,nx) - B.z(ny-1,nx))*dt/dy;
				J.x(ny,nx) = 0;
			}	
		}

		// Ey(y, x+1/2) at t+1 ------------------------------------------------------
		#pragma omp for
		for (int ny = E.iy_first(Y); ny < E.iy_last(Y); ++ny) { 
			for (int nx = E.ix_first(Y); nx < E.ix_last(Y); ++nx) {
				E.y(ny,nx) += -(J.y(ny,nx))*dt - (B.z(ny,nx) - B.z(ny,nx-1))*dt/dx;
				J.y(ny,nx) = 0;
			}		
		}

		// Ez(y+1/2,x+1/2) at t+1 ---------------------------------------------------
		#pragma omp for
		for (int ny = E.iy_first(Z); ny < E.iy_last(Z); ++ny) {
			for (int nx = E.ix_first(Z); nx < E.ix_last(Z); ++nx) {
				E.z(ny,nx) += -(J.z(ny,nx))*dt + ((B.y(ny,nx+1) - B.y(ny,nx))/dx
												- (B.x(ny+1,nx) - B.x(ny,nx))/dy)*dt;
				J.z(ny,nx) = 0;
			}	
		}
	}
}