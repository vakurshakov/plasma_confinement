EXECUTABLE := simulation.out

RESDIR := bin
OBJDIR := bin-int

# precompiled header
PCH := src/pch.h

# main
SRCS := src/main.cpp

# commands
SRCS +=                                       \
  src/command/set_particles.cpp               \
  src/command/copy_coordinates.cpp            \
  src/command/clone_layer_particles.cpp       \
  src/command/magnetic_field_half_step.cpp    \
  src/command/set_Bz_distribution.cpp         \

# diagnostics
SRCS +=                                       \
  src/diagnostics/energy.cpp                  \
  src/diagnostics/whole_field.cpp             \
  src/diagnostics/field_on_segment.cpp        \
  src/diagnostics/field_at_point.cpp          \
  src/diagnostics/chosen_particles.cpp        \
  src/diagnostics/distribution_moment.cpp     \
  src/diagnostics/x0_distribution_function.cpp\
  src/diagnostics/diagnostics_builder.cpp     \

# fields
SRCS +=                                       \
  src/fields/fields.cpp                       \
  src/fields/fields_builder.cpp               \
  src/fields/add_Bz0.cpp                      \
  src/fields/open_boundaries_processor.cpp    \

# filewriters
SRCS += src/file_writers/bin_file.cpp

# managers
SRCS += src/managers/manager.cpp

# particles
SRCS +=                                                \
  src/particles/particles.cpp                          \
  src/particles/particles_builder.cpp                  \
  src/particles/particles_load.cpp                     \
  src/particles/particle-boundary_processor.cpp        \
  src/particles/particle/particles_form-factors.cpp    \
  src/particles/particle/point_bound_interact.cpp      \

# solvers
SRCS +=                                                \
  src/solvers/FDTD.cpp                                 \
  src/solvers/Boris_pusher.cpp                         \
  src/solvers/Esirkepov_density_decomposition.cpp      \
  src/solvers/concrete_point_interpolation.cpp         \

# utils
SRCS +=                                                    \
  src/utils/log.cpp                                        \
  src/utils/time_manager.cpp                               \
  src/utils/transition_layer/parameter_function.cpp        \
  src/utils/transition_layer/particles_distribution.cpp    \

# vectors
SRCS +=                                                \
  src/vectors/vector_classes.cpp                       \
  src/vectors/vector3_field.cpp                        \

OBJS := $(SRCS:%.cpp=$(OBJDIR)/%.o)
DEPS := $(SRCS:%.cpp=$(OBJDIR)/%.d)

all: $(OBJDIR)/$(PCH).gch $(RESDIR)/$(EXECUTABLE)

-include $(DEPS)

# creates a directory for the target if it doesn't exist
MKDIR=@mkdir -p $(@D)

$(OBJDIR)/$(PCH).gch: $(PCH)
	@echo -e "\033[0;33m\nCompiling header src/pch.h.\033[0m"
	$(MKDIR)
	$(CXX) $(INC_PATH) $< -o $@

$(RESDIR)/$(EXECUTABLE): $(OBJS)
	@echo -e "\033[0;33m\nCreating the resulting binary.\033[0m"
	$(MKDIR)
	$(CXX) $(CFLAGS) $(LIB_PATH) $^ -Wl,-rpath=$(SPDLOG_DIR)/bin $(LIBS) -o $@

$(OBJDIR)/%.o: %.cpp message_compiling
	$(MKDIR)
	$(CXX) $(CFLAGS) $(INC_PATH) -c $< -o $@

$(OBJDIR)/%.d: %.cpp
	$(MKDIR)
	@$(CXX) $(CFLAGS) $(INC_PATH) $< -MM -MT $(@:$(OBJDIR)/%.d=$(OBJDIR)/%.o) >$@

.PHONY: clean
clean:
	@rm $(OBJDIR)/$(PCH).gch $(DEPS) $(OBJS) $(RESDIR)/$(EXECUTABLE)

# to prevent multiple messages
.INTERMEDIATE: message_compiling
message_compiling:
	@echo -e "\033[0;33m\nCompiling other files from src/**.\033[0m"
