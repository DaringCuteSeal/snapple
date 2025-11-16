# Define the C++ compiler
CXX = g++

# Compiler flags
CXXFLAGS = -O3 -Wall -I./deps/raylib-cpp/include -I./deps/raudio/src

# Linker flags
LDFLAGS = -lraylib

# Define the executable name
TARGET = snapple

# Define source files
SRCS = src/backend/components.cpp src/game/game.cpp src/game/intro_scene.cpp src/main.cpp

SRCS_TEST = src/backend/components.cpp tests/tests.cpp

# Define object files
OBJS = $(SRCS:.cpp=.o)

OBJS_TEST = $(SRCS_TEST:.cpp=.o)

# Default target: build the executable
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(TARGET)

# Rule for compiling .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target: remove generated files
clean:
	rm -f $(OBJS) $(TARGET) $(OBJS_TEST) test_backend

# Test target
test: $(OBJS_TEST)
	$(CXX) $(OBJS_TEST) $(LDFLAGS) -lgtest -o testsuite
	./testsuite
