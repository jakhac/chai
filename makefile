VERSION=2.5.5

CXX=g++
EXE=chai_$(VERSION)_$(CXX).exe

optimize=yes
debug=no
bits=64
prefetch=yes
popcnt=yes
sse=yes
sse2=yes
ssse3=yes
sse41=yes
avx2=yes
pext=yes

CXXFLAGS+=-Wcast-qual -fno-exceptions
DEPENDFLAGS+=-std=c++17

# Set flags according to optimzization settings
ifeq ($(INFO),yes)
	CXXFLAGS+=-DINFO=
endif

ifeq ($(optimize),yes)
	CXXFLAGS+=-O3
endif

ifeq ($(bits),64)
	CXXFLAGS+=-DIS_64_BIT
endif

ifeq ($(prefetch),yes)
	ifeq ($(sse),yes)
		CXXFLAGS+=-msse
		DEPENDFLAGS+=-msse
	endif
else
	CXXFLAGS+=-DNO_PREFETCH
endif

ifeq ($(popcnt),yes)
	ifeq ($(bits),64)
		CXXFLAGS+=-msse3 -mpopcnt -DUSE_POPCNT
	else
		CXXFLAGS+=-mpopcnt -DUSE_POPCNT
	endif
endif

ifeq ($(avx2),yes)
	CXXFLAGS+=-DUSE_AVX2
	CXXFLAGS+=-mavx2
endif

ifeq ($(pext),yes)
	CXXFLAGS+=-DUSE_PEXT
	CXXFLAGS+=-mbmi -mbmi2
endif

CXXFLAGS+=-flto
LDFLAGS+=$(CXXFLAGS)

BUILD_DIR := ./build
SRC_DIR := ./chai

CPP=$(wildcard $(SRC_DIR)/*.cpp)

# All .o files go to build dir.
OBJ=$(CPP:%.cpp=$(BUILD_DIR)/%.o)
# Gcc/Clang will create these .d files containing dependencies.
DEP=$(OBJ:%.o=%.d)

# Default target: chai.exe
$(EXE):$(BUILD_DIR)/$(EXE)

# Target binary depends on all .o files.
$(BUILD_DIR)/$(EXE): $(OBJ)
	$(CXX) $(LDFLAGS) $^ -o $@

# Build out files from src with dependecies
$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(LDFLAGS) -MMD -c $< -o $@

# Remove all generated files.
clean:
	del /Q $(subst /,\,$(BUILD_DIR)\$(EXE)) 2>nul
	del /Q $(subst /,\,.\$(OBJ)) 2>nul
	del /Q $(subst /,\,.\$(DEP)) 2>nul

all:
	$(MAKE) $(EXE)

# Include all .d files
-include $(DEP)

.PHONY: clean all
