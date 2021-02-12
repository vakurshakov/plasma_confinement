//-------- particles parameters --------------------------------------------------------
	const int spline_width = 4;
	const double Np = 25;
	const double mec2 = 511;

	const double e =  1;
	const double me = 1;
	const double Tx = 1;				// in keV
	const double Ty = 1;
	const double Tz = 0;
	
	const double NEUTRONS_CHARGE = 0;
	const double NEUTRONS_MASS = 1838;	// масс электрона


//-------- discrete modeling constants -------------------------------------------------
	const double d = 0.04;					// это безразмерное ^d, а размерное
	const double dx = d, dy = d;			// d и [d] = [^d * c/wp] = m 
	const double dt = 0.02;
	const size_t TIME = 10000;				// это безразмерное ^t, [t] = [^t * wp^-1] = s
	const size_t SIZE_X = 8, SIZE_Y = 8;
	const size_t SRC_POS_X = SIZE_X/2, SRC_POS_Y = SIZE_Y/2;