all: 2d_simulation

MAIN = main.o
VECTORS = vector_classes.o vector3_field.o 
SOLVERS = FDTD.o Boris_pusher.o Esirkepov_density_decomposition.o
PARTICLES = particles_form-factors.o boundaries_for_particle.o
DIAGNOSTICS = phase_diagram.o
.PHONY: clean, run

2d_simulation: $(VECTORS) $(PARTICLES) $(SOLVERS) $(DIAGNOSTICS) $(MAIN)
	g++ -fopenmp $(VECTORS) $(PARTICLES) $(SOLVERS) $(DIAGNOSTICS) $(MAIN) -o ../2d_simulation
	make clean
	make run

main.o:
	g++ -c -fopenmp main.cpp

vector_classes.o:
	g++ -c ./vectors/vector_classes.cpp

vector3_field.o:
	g++ -c ./vectors/vector3_field.cpp

Boris_pusher.o:
	g++ -c ./solvers/Boris_pusher.cpp

FDTD.o:
	g++ -c -fopenmp ./solvers/FDTD.cpp

Esirkepov_density_decomposition.o:
	g++ -c -fopenmp ./solvers/Esirkepov_density_decomposition.cpp

boundaries_for_particle.o:
	g++ -c ./particles/boundaries_for_particle.cpp

particles_form-factors.o:
	g++ -c ./particles/particles_form-factors.cpp

phase_diagram.o:
	g++ -c -fopenmp ./diagnostics/phase_diagram.cpp

run:
	../2d_simulation.exe

clean:
	del $(VECTORS) $(PARTICLES) $(SOLVERS) $(DIAGNOSTICS) $(MAIN) /Q