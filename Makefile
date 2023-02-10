# OBJ files to compile as part of the project
#OBJS    = $(wildcard *.c)
OBJS    = hello_world.cpp
CC      = g++

# -w supress all warnings
CFLAGS  = -w

# libaries we're linking against
LFLAGS  = -lSDL2

BIN = hello_world

# Targets (requires real <TAB> indentation)
all : $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(LFLAGS) -o $(BIN)

