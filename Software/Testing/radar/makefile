# Name of your executable
TARGET = lidar

# Compiler and flags
CC = clang
CFLAGS = -Wall -Wextra -lm

# Source files (adjust if you have more)
SRCS = radar.c map.c threads.c

# Object files (compiler will create these)
OBJS = $(SRCS:.c=.o) 

# The main build rule
all: $(TARGET)

# Rule to link object files into the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Rule to create object files from source files 
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Cleaning up
clean:
	rm -f $(OBJS) $(TARGET)
