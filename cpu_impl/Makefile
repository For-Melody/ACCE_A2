# Note: You can modify this file, but we will test your code with this makefile

# Compilers
CC=gcc

# Flags for optimization and libs
FLAGS=-O3 -Wall 
LIBS=-lm

# Targets to build
OBJS=KMEANS_seq

# make all targets
all: $(OBJS)

KMEANS.o: KMEANS.c
	$(CC) $(FLAGS) $(DEBUG) -c $< -o $@

KMEANS_seq.o: KMEANS_seq.c
	$(CC) $(FLAGS) $(DEBUG) -c $< -o $@

KMEANS_seq: KMEANS.o	KMEANS_seq.o 
	$(CC) $(DEBUG) $^ $(LIBS) -o $@

# Remove the target files
clean:
	rm -rf $(OBJS) *.o

# Compile in debug mode
debug:
	make DEBUG="-DDEBUG -g" FLAGS= all

