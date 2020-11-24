all: 2d_simulation

MAIN = main.o
VECTORS = vector_classes.o vector3_field.o 
SOLVERS = FDTD.o Boris_pusher.o Esirkepov_density_decomposition.o
PARTICLES = particles_form-factors.o boundaries_for_particle.o
.PHONY: clean, run

2D_Simulation: $(MAIN) $(VECTORS) $(SOLVERS) $(PARTICLES)
	g++ -fopenmp $(MAIN) $(VECTORS) $(SOLVERS) $(PARTICLES) -o 2d_simulation
	make clean
	make run

main.o:
	g++ -c -fopenmp main.cpp

Vector_classes.o:
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

Particles_form-factors.o:
	g++ -c ./particles/particles_Form-factors.cpp

run:
	./2d_simulation.exe

clean:
	del $(MAIN) $(VECTORS) $(SOLVERS) $(PARTICLES) /Q