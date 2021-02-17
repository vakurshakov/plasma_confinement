//-------- particles parameters --------------------------------------------------------
	const int spline_width = 4;
	const double Np = 24;
	const double mec2 = 511;

	const double e =  1;
	const double me = 1;
	const double Tx = 1;				// in keV
	const double Ty = 1;
	const double Tz = 0;
	
	const double nb = 0.05;
	const double np = 1 - nb;	

//-------- discrete modeling constants -------------------------------------------------
	const double d = 0.04;					// это безразмерное ^d, а размерное
	const double dx = d, dy = d;			// d и [d] = [^d * c/wp] = m 
	const double dt = 0.02;
	const int TIME = 10000;				// это безразмерное ^t, [t] = [^t * wp^-1] = s
	const int SIZE_X = 32, SIZE_Y = 4;
	const int SRC_POS_X = SIZE_X/2, SRC_POS_Y = SIZE_Y/2;