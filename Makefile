# Compile and link FILES into executable BIN
FILES = $(wildcard *.cpp)
UTILS = $(wildcard utils/*.cpp)
BIN = larp

# Compile and link SAMPLE_FILES
# Define BINS using patten substitution
SAMPLE_FILES = $(wildcard samples/*.cpp)
SAMPLE_BINS  = $(sort $(patsubst samples/%.cpp,bin/%,$(SAMPLE_FILES))) 
BIN_DIR 	 = ./bin

# Compiler and compiler flags
CC      = g++
CFLAGS  = -Wall

# Linker flag to link with SDL2 library
LFLAGS  = -lSDL2 -lSDL2_image

# Target: prereqs 
# <TAB> rules requires indentation

# `make` runs the first rule `all` when no target is specified
# `all` has no commands, except to run the BIN and SAMPLE_BINS 
# rules when those files are missing or have changed (newer timestamp than `all`)
all: $(SAMPLE_BINS) $(BIN)

# Generate the SAMPLE_BINS using automatic variables
# i.e. CC CFLAGS LFLAGS %.cpp -o %.bin
bin/%: samples/%.cpp
	@echo "Compiling samples..."
	$(CC) $< $(CFLAGS) $(LFLAGS) -o $@  

# Create executable BIN 
$(BIN): $(FILES) $(UTILS)
	@echo "Compiling executable..."
	$(CC) $^ $(CFLAGS) $(LFLAGS) -o $@  

clean:
	rm -rf $(BIN) $(SAMPLE_BINS) 

.PHONY: all clean
