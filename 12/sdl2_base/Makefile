# Compiler and flags
CXX = /usr/bin/g++       # Specify the full path to the correct g++
CXXFLAGS = -std=c++98 -g -Wall -I./SDL2/include -L./SDL2/lib64
LDFLAGS = -lSDL2 -lSDL2_image

# Project name and source files
TARGET = main
SRC = main.cpp

# Build rules
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)
	./main

# Clean up
clean:
	rm -f $(TARGET)

# Run the project
run: $(TARGET)
	./$(TARGET)
