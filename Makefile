SIMULATION_DIR := $(PWD)
SPDLOG_DIR     := $(PWD)/external/spdlog
JSON_DIR       := $(PWD)/external/json

INC_PATH :=
INC_PATH += -I$(SIMULATION_DIR)/
INC_PATH += -I$(SPDLOG_DIR)/include
INC_PATH += -I$(JSON_DIR)/include

LIB_PATH :=
LIB_PATH += -L$(SPDLOG_DIR)/lib

LIBS += -lspdlog

CXX    := g++
CFLAGS :=
CFLAGS += -std=c++20 -fPIC -fopenmp -pthread

# `filter X, A B` return those of A, B that are equal to X
ifeq ($(VERSION), $(filter $(VERSION), "DEBUG" ""))
CFLAGS += -O0 -ggdb -Wall -pedantic
endif

ifeq ($(VERSION), RELEASE)
CFLAGS += -O3
endif


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
  src/command/ionize_particles.cpp            \

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
  src/particles/particles_boundary_interaction.cpp     \
  src/particles/particle/particles_form-factors.cpp    \
  src/particles/particle/point_boundary_interaction.cpp\

# solvers
SRCS +=                                                \
  src/solvers/FDTD.cpp                                 \
  src/solvers/simple_interpolation.cpp                 \
  src/solvers/Boris_pusher.cpp                         \
  src/solvers/Esirkepov_density_decomposition.cpp      \

# utils
SRCS +=                                                    \
  src/utils/log.cpp                                        \
  src/utils/time_manager.cpp                               \
  src/utils/simulation_backup.cpp                          \
  src/utils/configuration_backup.cpp                       \
  # src/utils/transition_layer/table_function.cpp            \
  # src/utils/transition_layer/parameters_table.cpp          \
  # src/utils/transition_layer/particles_distribution.cpp    \

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
	$(CXX) $(CFLAGS) $(INC_PATH) $< -o $@

$(RESDIR)/$(EXECUTABLE): $(OBJS)
	@echo -e "\033[0;33m\nCreating the resulting binary.\033[0m"
	$(MKDIR)
	$(CXX) $(CFLAGS) $(LIB_PATH) $^ -Wl,-rpath=$(SPDLOG_DIR)/lib $(LIBS) -o $@

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
