all: 2d_simulation

MAIN 		=  main.o
VECTORS 	= vector_classes.o vector3_field.o 
SOLVERS 	= solvers_manager.o FDTD.o Boris_pusher.o Esirkepov_density_decomposition.o circular_current.o
PARTICLES 	= class_particles.o particles_form-factors.o boundaries_for_particle.o
DIAGNOSTICS = diagnostics.o energy.o field_configuration.o field_at_point.o phase_diagram.o

KEYS  = -c -fopenmp

.PHONY: clean, clean_o, run

2d_simulation: $(VECTORS) $(PARTICLES) $(SOLVERS) $(DIAGNOSTICS) $(MAIN)
	g++ -fopenmp $(VECTORS) $(PARTICLES) $(SOLVERS) $(DIAGNOSTICS) $(MAIN) -o ../2d_simulation
	make clean
	make run


main.o:
	g++ $(KEYS) ./main.cpp

vector_classes.o:
	g++ $(KEYS) ./vectors/vector_classes.cpp

vector3_field.o:
	g++ $(KEYS) ./vectors/vector3_field.cpp

solvers_manager.o:
	g++ $(KEYS) ./solvers/solvers_manager.cpp

Boris_pusher.o:
	g++ $(KEYS) ./solvers/Boris_pusher.cpp

FDTD.o:
	g++ $(KEYS) ./solvers/FDTD.cpp

Esirkepov_density_decomposition.o:
	g++ $(KEYS) ./solvers/Esirkepov_density_decomposition.cpp

circular_current.o:
	g++ $(KEYS) ./solvers/circular_current.cpp

class_particles.o:
	g++ $(KEYS) ./particles/class_particles.cpp

boundaries_for_particle.o:
	g++ $(KEYS) ./particles/boundaries_for_particle.cpp

particles_form-factors.o:
	g++ $(KEYS) ./particles/particles_form-factors.cpp

energy.o:
	g++ $(KEYS) ./diagnostics/energy.cpp
	
field_configuration.o:
	g++ $(KEYS) ./diagnostics/field_configuration.cpp

field_at_point.o:
	g++ $(KEYS) ./diagnostics/field_at_point.cpp

phase_diagram.o:
	g++ $(KEYS) ./diagnostics/phase_diagram.cpp

diagnostics.o:
	g++ $(KEYS) ./diagnostics/diagnostics.cpp

run:
	../2d_simulation.exe

clean:
	del *.o /Q