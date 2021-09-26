PROG ?= test

CC 	= g++
CFLAGs   = -std=c++17 -g -O0 -fopenmp -Wall -pedantic # DEBUG VERSION
#CFLAGs = -std=c++17 -O2 -fopenmp # RELEASE VERSION

OBJDIR = ../obj
DEPDIR = ../dep

VPATH = diagnostics/ fields/ file_writers/ managers/ particles/ particles/particle/ solvers/ vectors/ 

MAIN 		= main.cpp
VECTORS 	= vector_classes.cpp vector3_field.cpp
PARTICLES 	= particles_form-factors.cpp point_bound_interact.cpp
SOLVERS 	= FDTD.cpp Boris_pusher.cpp Esirkepov_density_decomposition.cpp
MANAGERS 	= fields.cpp fields_builder.cpp particles.cpp \
				particles_builder.cpp particles_load.cpp manager.cpp
ADDITIONALS	= add_Bz0.cpp add_ion_density.cpp add_ion_current.cpp
DIAGNOSTICS = energy.cpp whole_field.cpp field_along_the_line.cpp \
				field_at_point.cpp phase_diagram.cpp density.cpp
FILEWRITERS	= txt_file.cpp bin_file.cpp

SRCs = $(VECTORS) $(PARTICLES) $(MANAGERS) $(ADDITIONALS) $(FILEWRITERS) $(DIAGNOSTICS) $(SOLVERS) $(MAIN)
OBJs = $(SRCs:%.cpp=$(OBJDIR)/%.o)
DEPs = $(OBJs:$(OBJDIR)/%.o=$(DEPDIR)/%.d)

all: create_directories $(PROG)

$(PROG): $(OBJs)
	@$(CC) $(CFLAGs) $^ -o ../$@

$(DEPDIR)/%.d: %.cpp
	@$(CC) $(CFLAGs) $< -MM -MT $(@:$(DEPDIR)/%.d=$(OBJDIR)/%.o) >$@

-include $(DEPs)

$(OBJDIR)/%.o: %.cpp
	$(CC) $(CFLAGs) -c $< -o $@

.PHONY: create_directories
create_directories:
	@mkdir -p $(OBJDIR) $(DEPDIR) 

.PHONY: clean
clean:
	@rm $(DEPs) $(OBJs) ../$(PROG)

.PHONY: cleandeps
cleandep:
	@rm $(DEPs)