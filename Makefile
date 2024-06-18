# ---------------------------------------
# CONFIGURATION BEGIN
# ---------------------------------------

# entry point for the program and target name
MAIN := main.cpp

CACHE_SRCS := cache.cpp testbench.cpp


# target name
TARGET := cache_simulator

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

all: cache_simulator rahmenprogramm
	$(CXX) cache_simulator.o rahmenprogramm.o -o cache-simulator

rahmenprogramm:
	$(CC) -c rahmenprogramm.c -o rahmenprogramm.o

cache_simulator: CXXFLAGS += -g
cachesimulator: $(TARGET)

$(TARGET): $(MAIN) $(CACHE_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGET) *.o

.PHONY: cache_simulator clean