SIMULATION_DIR := $(PWD)
SPDLOG_DIR := $(PWD)/external/spdlog

INC_PATH :=
INC_PATH += -I$(SIMULATION_DIR)/
INC_PATH += -I$(SPDLOG_DIR)/include

LIB_PATH :=
LIB_PATH += -L$(SPDLOG_DIR)/bin

LIBS += -lspdlog

CXX    := g++
CFLAGS :=
CFLAGS += -std=c++20 -fopenmp -pthread

# `filter X, A B` return those of A, B that are equal to X
ifeq ($(VERSION), $(filter $(VERSION), "DEBUG" ""))
CFLAGS += -O0 -ggdb -Wall -pedantic
endif

ifeq ($(VERSION), RELEASE)
CFLAGS += -O3
endif

export CXX CFLAGS SIMULATION_DIR SPDLOG_DIR INC_PATH LIB_PATH LIBS

all:
	@$(MAKE) -C external/spdlog -f spdlog.mk
	@$(MAKE) -f simulation.mk

.PHONY: clean
clean:
	@$(MAKE) -C external/spdlog -f spdlog.mk clean
	@$(MAKE) -f simulation.mk clean