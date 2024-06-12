# Setting the compiler to gcc
CC = gcc

# Setting the object file to be generated
OBJ = program.o

# Rule for building the program
program: $(OBJ)
	$(CC) -o program $(OBJ)

# Rule for building the object file
$(OBJ): program.c
	$(CC) -c program.c

clean:
	rm -f $(OBJ) program