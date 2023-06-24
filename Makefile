# Compiler
CC = gcc

# Compiler flags
CFLAGS = `pkg-config gtk+-3.0 --cflags`

# Linker flags
LDFLAGS = `pkg-config gtk+-3.0 --libs` -lcurl

# Source files
SRC = mainFile.c wiki.c cJSON.c fileIO.c readFile.c tokenizer.c

# Object files
OBJ = $(SRC:.c=.o)

# Executable name
EXEC = mainFile

# Build rule
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run rule
run: $(EXEC)
	./$(EXEC)

# Clean rule
clean:
	rm -f $(OBJ) $(EXEC)
