# ---------------------------------------
# CONFIGURATION BEGIN
# ---------------------------------------

# target name
EXECUTABLE := cache_simulation

# entry point for the program and target name
MAINC := src/rahmenprogramm.c
MAINCXX := src/main.cpp

# cache related files
CACHE := src/cache.hpp
CACHEIMP := src/cache.cpp
MEMORY := src/memory.hpp
MEMORYIMP := src/memory.cpp
CACHELINE := src/cacheLine.h
CACHELINEIMP := src/cacheLine.cpp
DIRECTMAPPEDCACHE := src/directMappedCache.h
DIRECTMAPPEDCACHEIMP := src/directMappedCache.cpp

# cpu related files
CPU := src/cpu.hpp
CPUIMP := src/cpu.cpp

# tracefiles
TRACEFILES := *.vcd

# simulation handler in c++
SIM := src/simulation.cpp

# Path to our systemc installation
SCPATH = $(SYSTEMC_HOME)

# Define the object files
C_OBJS := $(MAINC:.c=.o)
CXX_OBJS := $(MAINCXX:.cpp=.o) $(CACHEIMP:.cpp=.o) $(CPUIMP:.cpp=.o) $(SIM:.cpp=.o) $(CACHELINEIMP:.cpp=.o) $(DIRECTMAPPEDCACHEIMP:.cpp=.o) $(MEMORYIMP:.cpp=.o)

# Additional flags for the compiler
# -std=c++14: This flag specifies the C++ standard to be used by the compiler. In this case, it tells the compiler to use the C++14 standard.
# -I$(SCPATH)/include: The -I flag adds a directory to the list of directories to be searched for header files during compilation. Here, $(SCPATH) is a variable that holds the path to the sysC directory, and /include is appended to point to the include directory within that path. This tells the compiler to look in $(SCPATH)/include for any header files (*.h or *.hpp) our code might include.
# -L$(SCPATH)/lib: The -L flag adds a directory to the list of directories to be searched for libraries during linking. Similar to the -I flag, $(SCPATH) is a variable that holds the path to the sysC directory, and /lib is appended to point to the library directory within that path. This tells the linker to look in $(SCPATH)/lib for any libraries it needs to link against our program.d
# -lsystemc: The -l flag specifies a library to be linked. In this case, -lsystemc tells the linker to link against the systemc library. The linker will search for a file named libsystemc.so (on Unix-like systems) or libsystemc.a in the directories specified by the -L flags.
# -lm: Similar to -lsystemc, the -lm flag tells the linker to link against the math library. This is a standard library provided by the system for mathematical functions, and the linker will search for a file named libm.so (on Unix-like systems) or libm.a.
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

# Default to release build
all: release

# Debug build
debug: CXXFLAGS += -g
debug: $(EXECUTABLE)

# Release build
release: CXXFLAGS += -O2
release: $(EXECUTABLE)

# Compile the main C file and the sim file into an object file
$(MAINC:.c=.o): $(MAINC)
	$(CC) $(CFLAGS) -c $< -o $@

$(SIM:.cpp=.o): $(SIM)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile the main C++ file into an object file
$(MAINCXX:.cpp=.o): $(MAINCXX)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile the C++ files into object files
$(CACHEIMP:.cpp=.o): $(CACHEIMP)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(CPUIMP:.cpp=.o): $(CPUIMP)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link all object files into the final executable
$(EXECUTABLE): $(C_OBJS) $(CXX_OBJS)
	$(CXX) $(CFLAGS) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	rm -f $(C_OBJS) $(CXX_OBJS)

# Clean up the build
clean:
	rm -f $(C_OBJS) $(CXX_OBJS) $(EXECUTABLE) $(TRACEFILES)