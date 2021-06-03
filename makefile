CXX := g++
TARGET_EXEC := chai_g++

BUILD_DIR := ./build
SRC_DIRS := ./chai

# Find all the C and C++ files we want to compile
# SRCS := $(shell find $(SRC_DIRS) -name *.cpp)
SRCS := $(wildcard $(SRC_DIRS)/*.cpp)

# String substitution for every C/C++ file.
# As an example, hello.cpp turns into ./build/hello.cpp.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# String substitution (suffix version without %).
# As an example, ./build/hello.cpp.o turns into ./build/hello.cpp.d
DEPS := $(OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header files
# INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_DIRS := $(wildcard $(SRC_DIRS)/*.d)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# The -MMD and -MP flags together generate Makefiles for us!
# These files will have .d instead of .o as the output.
CPPFLAGS := $(INC_FLAGS) -MMD -MP
CPPFLAGS += -Wswitch-bool -std=c++14 -march=native -O3

# The final build step.
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Build step for C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp createDir
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

createDir:
	rmdir /S /Q $(subst ./,,$(BUILD_DIR))
	mkdir build\chai

.PHONY: clean
clean:
	rmdir /S /Q $(subst ./,,$(BUILD_DIR))

-include $(DEPS)