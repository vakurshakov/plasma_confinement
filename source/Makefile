all: 2d_simulation

MAIN 		= main.o
VECTORS 	= vector_classes.o vector3_field.o 
SOLVERS 	= FDTD.o Boris_pusher.o Esirkepov_density_decomposition.o
ADDITIONALS	= add_circular_current.o
PARTICLES 	= class_particles.o particles_form-factors.o boundaries_for_particle.o
DIAGNOSTICS = energy.o whole_field.o field_at_point.o phase_diagram.o
MANAGERS 	= fields_manager.o particles_manager.o

KEYS  = -c -fopenmp

.PHONY: clean, clean_o, run

2d_simulation: $(VECTORS) $(PARTICLES) $(SOLVERS) $(MANAGERS) $(ADDITIONALS) $(DIAGNOSTICS) $(MAIN)
	g++ -fopenmp $(VECTORS) $(PARTICLES) $(SOLVERS) $(MANAGERS) $(ADDITIONALS) $(DIAGNOSTICS) $(MAIN) -o ../2d_simulation
	make clean
	make run


main.o:
	g++ $(KEYS) ./main.cpp

vector_classes.o:
	g++ $(KEYS) ./vectors/vector_classes.cpp

vector3_field.o:
	g++ $(KEYS) ./vectors/vector3_field.cpp

Boris_pusher.o:
	g++ $(KEYS) ./solvers/Boris_pusher.cpp

FDTD.o:
	g++ $(KEYS) ./solvers/FDTD.cpp

Esirkepov_density_decomposition.o:
	g++ $(KEYS) ./solvers/Esirkepov_density_decomposition.cpp

class_particles.o:
	g++ $(KEYS) ./particles/class_particles.cpp

boundaries_for_particle.o:
	g++ $(KEYS) ./particles/boundaries_for_particle.cpp

particles_form-factors.o:
	g++ $(KEYS) ./particles/particles_form-factors.cpp

fields_manager.o:
	g++ $(KEYS) ./managers/fields_manager.cpp

add_circular_current.o:
	g++ $(KEYS) ./managers/fields_additionals/add_circular_current.cpp

particles_manager.o:
	g++ $(KEYS) ./managers/particles_manager.cpp

energy.o:
	g++ $(KEYS) ./diagnostics/energy.cpp
	
whole_field.o:
	g++ $(KEYS) ./diagnostics/whole_field.cpp

field_at_point.o:
	g++ $(KEYS) ./diagnostics/field_at_point.cpp

phase_diagram.o:
	g++ $(KEYS) ./diagnostics/phase_diagram.cpp

run:
	../2d_simulation.exe

clean:
	del *.o /Q