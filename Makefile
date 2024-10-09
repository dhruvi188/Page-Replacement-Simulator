CC = gcc
CFLAGS = -Wall -Wextra -pedantic
SRC_DIR = src
BIN_DIR = bin

# Source files for each program
VMGEN_SRC = $(SRC_DIR)/vmgen.c
VMSIM_SRC = $(SRC_DIR)/vmsim.c
VMSTATS_SRC = $(SRC_DIR)/vmstats.c

# Executable names
VMGEN_EXE = $(BIN_DIR)/vmgen
VMSIM_EXE = $(BIN_DIR)/vmsim
VMSTATS_EXE = $(BIN_DIR)/vmstats

# Object files (not needed for this project, but left here for reference)
# OBJ = $(SRC:.c=.o)

# Default target
all: $(VMGEN_EXE) $(VMSIM_EXE) $(VMSTATS_EXE)

# Compile vmgen
$(VMGEN_EXE): $(VMGEN_SRC)
	$(CC) $(CFLAGS) $< -o $@

# Compile vmsim
$(VMSIM_EXE): $(VMSIM_SRC)
	$(CC) $(CFLAGS) $< -o $@

# Compile vmstats
$(VMSTATS_EXE): $(VMSTATS_SRC)
	$(CC) $(CFLAGS) $< -o $@

# Clean generated files
clean:
	rm -f $(VMGEN_EXE) $(VMSIM_EXE) $(VMSTATS_EXE)
