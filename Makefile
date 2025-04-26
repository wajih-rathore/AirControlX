CXX = g++
CXXFLAGS = -std=c++11 -Wall -pthread
INCLUDES = -Iinclude
SRC_DIR = src
OBJ_DIR = obj

# Find all source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp) main.cpp
# Generate object file names
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(filter $(SRC_DIR)/%.cpp,$(SRCS))) $(OBJ_DIR)/main.o

# Final executable
TARGET = aircontrolx

# Main target
all: directories $(TARGET)

# Make the build directory
directories:
	mkdir -p $(OBJ_DIR)

# Link the object files
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile main.cpp
$(OBJ_DIR)/main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Run the program
run: all
	./$(TARGET)

.PHONY: all directories clean run