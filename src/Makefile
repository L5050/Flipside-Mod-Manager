# Define the C++ compiler to use
CC = g++

# Define any compile-time flags
CFLAGS = -Wall -std=c++17

# Define the target file
TARGET = mod_manager

# Define the source file(s)
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	$(RM) $(TARGET)
