# ---------------------------------------
# CONFIGURATION BEGIN
# ---------------------------------------

# entry point for the program and target name
CACHE_SRCS := cache.cpp testbench.cpp main.cpp

RAHMENPROGRAMM_SRCS := rahmenprogramm.c


# Path to your systemc installation
SCPATH = ../systemc

# Additional flags for the compiler
CXXFLAGS := -std=c++14  -I$(SCPATH)/include -L$(SCPATH)/lib -lsystemc -lm

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
# Diese Zeilen fügen -rpath zu den Linker-Flags hinzu, außer wenn das Betriebssystem macOS (Darwin) ist. -rpath gibt dem Linker an, wo er zur Laufzeit nach Bibliotheken suchen soll.
UNAME_S := $(shell uname -s)

ifneq ($(UNAME_S), Darwin)
    CXXFLAGS += -Wl,-rpath=$(SCPATH)/lib
endif

all: link_all

rahmenprogramm: 
	$(CC) -g -o rahmenprogramm $(RAHMENPROGRAMM_SRCS)  

cache_simulator: CXXFLAGS += -g
cache_simulator: 
	$(CXX) -g $(CXXFLAGS) -o cache_simulator $(CACHE_SRCS)

rahmenprogramm.o: rahmenprogramm.c
	$(CC) -g -c rahmenprogramm.c -o rahmenprogramm.o

link_all: rahmenprogramm.o
	$(CXX) $(CXXFLAGS) -o linked_program rahmenprogramm.o cache.cpp main.cpp simulation.cpp testbench.cpp

debug: rahmenprogramm.o
	$(CXX) -g $(CXXFLAGS) -o linked_program rahmenprogramm.o cache.cpp main.cpp simulation.cpp testbench.cpp
	
clean:
	rm -f cache_simulator rahmenprogramm linked_program *.o

.PHONY: all clean rahmenprogramm cache_simulator link_all