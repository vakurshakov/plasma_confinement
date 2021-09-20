PROG ?= test.exe

VPATH = vectors/ particle/ managers/ managers/additionals/ solvers/ file_writers/ diagnostics/

MAIN 		= main.cpp
VECTORS 	= vector_classes.cpp vector3_field.cpp
PARTICLES 	= particles_form-factors.cpp point_bound_interact.cpp
SOLVERS 	= FDTD.cpp Boris_pusher.cpp Esirkepov_density_decomposition.cpp
MANAGERS 	= fields.cpp fields_builder.cpp particles.cpp \
				particles_builder.cpp particles_load.cpp manager.cpp
#COMMANDS	=
ADDITIONALS	= add_Bz0.cpp add_ion_density.cpp add_ion_current.cpp
DIAGNOSTICS = energy.cpp whole_field.cpp field_along_the_line.cpp \
				field_at_point.cpp phase_diagram.cpp density.cpp
FILEWRITERS	= txt_file.cpp bin_file.cpp


SRCs = $(VECTORS) $(PARTICLES) $(MANAGERS) $(ADDITIONALS) $(FILEWRITERS) $(DIAGNOSTICS) $(SOLVERS) $(MAIN)
OBJs = $(notdir $(patsubst %.cpp, %.o, $(SRCs)))


CXX 	= g++ -Wall -pedantic -ggdb -O2
CFLAGs  = -std=c++17 -fopenmp

.PHONY: clean

# Добавить складывание в директорию ../obj

$(PROG): $(OBJs)
	$(CXX) $(CFLAGs) -o ../$@ $^
	make clean

%.o: %.cpp
	$(CXX) $(CFLAGs) -c -o $@ $^

clean:
	del *.o 
