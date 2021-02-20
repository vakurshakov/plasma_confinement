#include <string>
#include <vector>

using namespace std;

//-------- particles constants ---------------------------------------------------------
	const double mec2 	= 511;
	const double e 		= 1;
	const double me 	= 1;
	const double mpr 	= 1836;

//-------- diagnostics -----------------------------------------------------------------
	const string head_folder = "../diagnostics/circular_current_test";
	const vector<string> names_of_diagnostics = { "energy",
												  "field configuration",
												  "field at point",
												  "phase diagram" };

//-------- solvers ---------------------------------------------------------------------
	const vector<string> solvers = { "FDTD",
									 "Boris_pusher",
									 "Esirkepov_density_decomposition",
									 "circular_current" };

//-------- configurarion ---------------------------------------------------------------
	const double Np = 8;
	const double Tx = 1;
	const double Ty = 1;
	const double Tz = 0;

	const string XY_distrib = "random";
	const double np 	= 0.5;
	const double v_inj 	= 0.5;
	const double Bz 	= 1;

	const int spline_width = 4;
	
	const double d = 0.04;
	const double dx = d, dy = d; 
	const double dt = 0.02;
	const int TIME = 1;
	const int SIZE_X = 1, SIZE_Y = 1;