enum SOLV_P { PUSHER, DECOMP };
enum CONF 	{ BOUND, n_, NP, XY_, P_ };

void Particles::initialization(vector<string> solver, vector<string> configuration,
	string test_name, map<string, vector<string>> diagnostics)
{
	// initialization of solvers
	if ( solvers[PUSHER] == "Boris_pusher" ) {
		Particle_push_ = Boris_pusher;
	}

	if (solvers[DECOMP] == "Esirkepov_density_decomposition" ) {
		Density_decomposition_ = Esirkepov_density_decomposition;
	}


	// initialization of configuration
	string boundaries = configuration[BOUND];

	if ( boundaries == "reflective" ) {
		x_boundary = reflective_Xboundary;
		y_boundary = reflective_Yboundary;
	}
	else if ( boundaries == "periodic" ) {
		x_boundary = periodic_Xboundary;
		y_boundary = periodic_Yboundary;
	}
	else if ( boundaries == "rh_pv" ) {
		x_boundary = reflective_Xboundary;
		y_boundary = periodic_Yboundary;
	}
	else if ( boundaries == "rv_ph" ) {
		x_boundary = periodic_Xboundary;
		y_boundary = reflective_Yboundary;
	}

	double n0 = stod(configuration[n0_]);
	double ni = stod(configuration[ni_]);
	double Np = stod(configuration[NP]);
	string XY_distrib = configuration[XY_];
	string P_distrib = configuration[P_];

	protons pr_(ni);
	electrons el_(n0, Np, XY_distrib, vector2(0,0));

	particles = { pr_, el_ };

	//initialization of diagnostics
	for (auto& name : diagnostics_name) {
		if ( name == "energy" ) {
			diagnostics.emplace_back(make_unique<energy>(test_name_ + "/" + name));
		}
		else if ( name == "phase_diagram" ) {
			diagnostics.emplace_back(make_unique<phase_diagram>(test_name_ + "/" + name));
		}
	}

	diagnostics.shrink_to_fit();
}
