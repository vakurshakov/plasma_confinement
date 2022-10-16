EXECUTABLE := simulation.out

CC := g++
CFLAGs := -std=c++20 -ggdb -O0 -fopenmp -Wall -pedantic # DEBUG VERSION
#CFLAGs = -std=c++20 -O3 -fopenmp # RELEASE VERSION

RESDIR := bin
OBJDIR := bin-int

VPATH := src/
VPATH += src/diagnostics/ src/fields/ src/file_writers/ src/managers/ src/solvers/
VPATH += src/vectors/ src/command/ src/particles/ src/particles/particle/ src/utils/ src/utils/transition_layer

# Precompiled header
PCH := pch.h

MAIN        := main.cpp
VECTORS     := vector_classes.cpp vector3_field.cpp
PARTICLES   := particles_form-factors.cpp point_bound_interact.cpp particle-boundary_processor.cpp
SOLVERS     := FDTD.cpp Boris_pusher.cpp Esirkepov_density_decomposition.cpp \
           concrete_point_interpolation.cpp
MANAGERS    := fields.cpp fields_builder.cpp particles.cpp \
            particles_builder.cpp particles_load.cpp time_manager.cpp manager.cpp
FIELDS      := add_Bz0.cpp open_boundaries_processor.cpp
DIAGNOSTICS := energy.cpp whole_field.cpp field_on_segment.cpp field_at_point.cpp \
            distribution_moment.cpp chosen_particles.cpp diagnostics_builder.cpp
COMMANDS    := set_particles.cpp copy_coordinates.cpp clone_layer_particles.cpp \
            magnetic_field_half_step.cpp set_Bz_distribution.cpp
FILEWRITERS := txt_file.cpp bin_file.cpp
UTILS       := parameter_function.cpp particles_distribution.cpp 

SRCs := $(COMMANDS) $(VECTORS) $(PARTICLES) $(MANAGERS) $(FIELDS) $(FILEWRITERS) $(DIAGNOSTICS) $(SOLVERS) $(MAIN) $(UTILS)
OBJs := $(SRCs:%.cpp=$(OBJDIR)/%.o)
DEPs := $(OBJs:$(OBJDIR)/%.o=$(OBJDIR)/%.d)

all: $(OBJDIR)/$(PCH).gch $(RESDIR)/$(EXECUTABLE)

-include $(DEPs)

$(OBJDIR)/$(PCH).gch: $(PCH)
	$(dir_guard)
	@echo -e "\033[0;33m\nCompiling header src/pch.h.\033[0m"
	$(CC) $(CFLAGs) $< -o $@

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
	@rm $(OBJDIR)/$(PCH).gch $(DEPs) $(OBJs) $(RESDIR)/$(EXECUTABLE)

# To prevent multiple messages
.INTERMEDIATE: message_compiling
message_compiling:
	@echo -e "\033[0;33m\nCompiling other files from src/**.\033[0m"
