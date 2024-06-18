all: rahmenprogramm simulation
	g++ simulation.o rahmenprogramm.o -o cache-simulator

# Compiling the simulation
simulation:
	g++ -c simulation.cpp -o simulation.o

# Compiling the Rahmenprogramm
rahmenprogramm:
	gcc -c rahmenprogramm.c -o rahmenprogramm.o

clean:
	rm -f *.o cache-simulator