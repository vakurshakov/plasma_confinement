PROG ?= 2d_simulation

MAIN 		= main.o
VECTORS 	= vector_classes.o vector3_field.o 
SOLVERS 	= FDTD.o Boris_pusher.o Esirkepov_density_decomposition.o
ADDITIONALS	= add_ion_current.o add_Bz0.o
PARTICLES 	= particles_form-factors.o boundaries_for_particle.o
DIAGNOSTICS = energy.o whole_field.o field_along_the_line.o field_at_point.o phase_diagram.o density.o
MANAGERS 	= fields.o particles.o particles_load.o

# setting all object files
OBJs = $(VECTORS) $(PARTICLES) $(SOLVERS) $(MANAGERS) $(ADDITIONALS) $(DIAGNOSTICS) $(MAIN)

CXX 	= g++ -Wall -pedantic -ggdb -O2
CFLAGs  = -std=c++17 -fopenmp

.PHONY: clean

$(PROG): $(OBJs)
	$(CXX) $(CFLAGs) $(OBJs) -o ../2d_simulation
	make clean

main.o:
	$(CXX) $(CFLAGs) -c -o $@ ./main.cpp

vector_classes.o:
	$(CXX) $(CFLAGs) -c -o $@ ./vectors/vector_classes.cpp

vector3_field.o:
	$(CXX) $(CFLAGs) -c -o $@ ./vectors/vector3_field.cpp

Boris_pusher.o:
	$(CXX) $(CFLAGs) -c -o $@ ./solvers/Boris_pusher.cpp

FDTD.o:
	$(CXX) $(CFLAGs) -c -o $@ ./solvers/FDTD.cpp

Esirkepov_density_decomposition.o:
	$(CXX) $(CFLAGs) -c -o $@ ./solvers/Esirkepov_density_decomposition.cpp

boundaries_for_particle.o:
	$(CXX) $(CFLAGs) -c -o $@ ./particles/boundaries_for_particle.cpp

particles_form-factors.o:
	$(CXX) $(CFLAGs) -c -o $@ ./particles/particles_form-factors.cpp

fields.o:
	$(CXX) $(CFLAGs) -c -o $@ ./managers/fields.cpp

add_ion_current.o:
	$(CXX) $(CFLAGs) -c -o $@ ./managers/fields_additionals/add_ion_current.cpp

add_Bz0.o:
	$(CXX) $(CFLAGs) -c -o $@ ./managers/fields_additionals/add_Bz0.cpp

particles.o:
	$(CXX) $(CFLAGs) -c -o $@ ./managers/particles.cpp

particles_load.o:
	$(CXX) $(CFLAGs) -c -o $@ ./managers/particles_load.cpp

energy.o:
	$(CXX) $(CFLAGs) -c -o $@ ./diagnostics/energy.cpp
	
whole_field.o:
	$(CXX) $(CFLAGs) -c -o $@ ./diagnostics/whole_field.cpp

field_along_the_line.o:
	$(CXX) $(CFLAGs) -c -o $@ ./diagnostics/field_along_the_line.cpp

field_at_point.o:
	$(CXX) $(CFLAGs) -c -o $@ ./diagnostics/field_at_point.cpp

phase_diagram.o:
	$(CXX) $(CFLAGs) -c -o $@ ./diagnostics/phase_diagram.cpp

density.o:
	$(CXX) $(CFLAGs) -c -o $@ ./diagnostics/density.cpp

clean:
	del *.o /Q
