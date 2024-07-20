# ---------------------------------------
# CONFIGURATION BEGIN
# ---------------------------------------

# target name
EXECUTABLE := cache_simulation

# entry point for the program
RAHMENPROGRAMM := src/rahmenprogramm.c

# simulation handler in c++
SIM := src/simulation.cpp

# SystemC:
CPU := src/cpu.cpp

CACHE := src/cache.cpp
DIRECTMAPPEDCACHE := src/directMappedCache.cpp
FOURWAYMAPPEDCACHE := src/fourwayMappedCache.cpp

MEMORY := src/memory.cpp

# tracefiles
TRACEFILES := *.vcd

# Path to the systemc installation
SCPATH = $(SYSTEMC_HOME)

# Additional flags for the compiler
CXXFLAGS := -std=c++14 -I"$(SCPATH)/include"
LDFLAGS := -L"$(SCPATH)/lib" -lsystemc -lm
CFLAGS := -std=c18

# ---------------------------------------
# CONFIGURATION END
# ---------------------------------------

# Determine if clang or gcc is available else exit
CXX := $(shell command -v g++ || command -v clang++)
ifeq ($(strip $(CXX)),)
    $(error Neither clang++ nor g++ is available. Exiting.)
endif

CC := $(shell command -v gcc || command -v clang)
ifeq ($(strip $(CC)),)
    $(error Neither gcc nor clang is available. Exiting.)
endif

# Add rpath except for MacOS
UNAME_S := $(shell uname -s)
ifneq ($(UNAME_S), Darwin)
    CXXFLAGS += -Wl,-rpath=$(SCPATH)/lib
endif

# Source files and object files
SOURCES_C := $(RAHMENPROGRAMM)
SOURCES_CXX := $(CPU) $(CACHE) $(DIRECTMAPPEDCACHE) $(FOURWAYMAPPEDCACHE) $(MEMORY) $(SIM)
OBJECTS_C := $(SOURCES_C:.c=.o)
OBJECTS_CXX := $(SOURCES_CXX:.cpp=.o)
OBJECTS := $(OBJECTS_C) $(OBJECTS_CXX)

# Default to release build
all: release

# Debug build
debug: CXXFLAGS += -g
debug: $(EXECUTABLE)

# Release build
release: CXXFLAGS += -O2
release: $(EXECUTABLE)

# Compile C source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C++ source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link all object files into the final executable
$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)

# Clean up the build
clean:
	rm -f $(EXECUTABLE) $(TRACEFILES) $(OBJECTS)

# Declare the targets that are not files
.PHONY: all debug release clean
