# OBJ files to compile as part of the project
#OBJS    = $(wildcard *.c)
OBJS    = larp.cpp
CC      = g++

# -w supress all warnings
CFLAGS  = -w

# libaries we're linking against
LFLAGS  = -lSDL2

BIN = larp.bin

# Targets (requires <TAB> indentation)
all: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(LFLAGS) -o $(BIN)
	./$(BIN)

clean:
	rm -rf $(BIN)

