EXECUTABLE := simulation.out

CC := g++
CFLAGs := -std=c++20 -ggdb -O0 -fopenmp -Wall -pedantic # DEBUG VERSION
#CFLAGs = -std=c++20 -O3 -fopenmp # RELEASE VERSION

RESDIR := ../bin
OBJDIR := ../bin-int

VPATH := diagnostics/ fields/ file_writers/ managers/ solvers/ vectors/ command/ 
VPATH += particles/ particles/particle/

# Precompiled header
PCH := pch.h

MAIN        := main.cpp
VECTORS     := vector_classes.cpp vector3_field.cpp
PARTICLES   := particles_form-factors.cpp point_bound_interact.cpp
SOLVERS     := FDTD.cpp Boris_pusher.cpp Esirkepov_density_decomposition.cpp \
           concrete_point_interpolation.cpp
MANAGERS    := fields.cpp fields_builder.cpp particles.cpp \
            particles_builder.cpp particles_load.cpp manager.cpp
ADDITIONALS := add_Bz0.cpp add_ion_density.cpp add_ion_current.cpp
DIAGNOSTICS := energy.cpp whole_field.cpp field_along_the_line.cpp \
            field_at_point.cpp distribution_moment.cpp chosen_particles.cpp single_field.cpp
COMMANDS    := set_particles.cpp copy_coordinates.cpp ionize_particles.cpp \
            magnetic_field_half_step.cpp
FILEWRITERS := txt_file.cpp bin_file.cpp

SRCs := $(COMMANDS) $(VECTORS) $(PARTICLES) $(MANAGERS) $(ADDITIONALS) $(FILEWRITERS) $(DIAGNOSTICS) $(SOLVERS) $(MAIN)
OBJs := $(SRCs:%.cpp=$(OBJDIR)/%.o)
DEPs := $(OBJs:$(OBJDIR)/%.o=$(OBJDIR)/%.d)

all: $(PCH).gch $(RESDIR)/$(EXECUTABLE)

-include $(DEPs) 

# Compiling header
$(PCH).gch: $(PCH)
	$(CC) $(CFLAGs) $<

# Creates a directory for the target if it doesn't exist
dir_guard=@mkdir -p $(@D)

$(RESDIR)/$(EXECUTABLE): $(OBJs)
	$(dir_guard)
	$(CC) $(CFLAGs) $^ -o $@

$(OBJDIR)/%.o: %.cpp
	$(dir_guard)
	$(CC) $(CFLAGs) -I../ -c $< -o $@

$(OBJDIR)/%.d: %.cpp
	$(dir_guard)
	@$(CC) $(CFLAGs) -I../ $< -MM -MT $(@:$(OBJDIR)/%.d=$(OBJDIR)/%.o) >$@

.PHONY: clean
clean:
	@rm $(OBJDIR)/$(DEPs) $(OBJDIR)/$(OBJs) $(RESDIR)/$(EXECUTABLE)
