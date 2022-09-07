EXECUTABLE := simulation.out

CC := g++
CFLAGs := -std=c++20 -ggdb -O0 -fopenmp -Wall -pedantic # DEBUG VERSION
#CFLAGs = -std=c++20 -O3 -fopenmp # RELEASE VERSION

RESDIR := bin
OBJDIR := bin-int

VPATH := src/
VPATH += src/diagnostics/ src/fields/ src/file_writers/ src/managers/ src/solvers/
VPATH += src/vectors/ src/command/ src/particles/ src/particles/particle/

# Precompiled header
PCH := pch.h

MAIN        := main.cpp
VECTORS     := vector_classes.cpp vector3_field.cpp
PARTICLES   := particles_form-factors.cpp point_bound_interact.cpp
SOLVERS     := FDTD.cpp Boris_pusher.cpp Esirkepov_density_decomposition.cpp \
           concrete_point_interpolation.cpp
MANAGERS    := fields.cpp fields_builder.cpp particles.cpp \
            particles_builder.cpp particles_load.cpp manager.cpp
ADDITIONALS := add_Bz0.cpp
DIAGNOSTICS := energy.cpp whole_field.cpp field_along_the_line.cpp field_at_point.cpp \
						distribution_moment.cpp chosen_particles.cpp single_field.cpp
COMMANDS    := set_particles.cpp copy_coordinates.cpp ionize_particles.cpp \
            magnetic_field_half_step.cpp
FILEWRITERS := txt_file.cpp bin_file.cpp

SRCs := $(COMMANDS) $(VECTORS) $(PARTICLES) $(MANAGERS) $(ADDITIONALS) $(FILEWRITERS) $(DIAGNOSTICS) $(SOLVERS) $(MAIN)
OBJs := $(SRCs:%.cpp=$(OBJDIR)/%.o)
DEPs := $(OBJs:$(OBJDIR)/%.o=$(OBJDIR)/%.d)

all: $(PCH).gch $(RESDIR)/$(EXECUTABLE)

-include $(DEPs) 

$(PCH).gch: $(PCH)
	@echo -e "\033[0;33m\nCompiling header src/pch.h.\033[0m"
	$(CC) $(CFLAGs) $<

# Creates a directory for the target if it doesn't exist
dir_guard=@mkdir -p $(@D)

$(RESDIR)/$(EXECUTABLE): $(OBJs)
	$(dir_guard)
	@echo -e "\033[0;33m\nCreating the resulting binary.\033[0m"
	$(CC) $(CFLAGs) $^ -o $@

$(OBJDIR)/%.o: %.cpp message_compiling
	$(dir_guard)
	$(CC) $(CFLAGs) -I./ -c $< -o $@

$(OBJDIR)/%.d: %.cpp
	$(dir_guard)
	@$(CC) $(CFLAGs) -I./ $< -MM -MT $(@:$(OBJDIR)/%.d=$(OBJDIR)/%.o) >$@

.PHONY: clean
clean:
	@rm $(DEPs) $(OBJs) $(RESDIR)/$(EXECUTABLE)

# To prevent multiple messages
.INTERMEDIATE: message_compiling
message_compiling:
	@echo -e "\033[0;33m\nCompiling files from src/**.\033[0m"
	