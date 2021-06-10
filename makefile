.DEFAULT_GOAL := default

VERSION = 2.5.5

EXE = chai_$(VERSION)_$(CXX).exe

optimize = yes
debug = no
bits = 64
prefetch = no
popcnt = no
sse = no
sse2 = no
ssse3 = no
sse41 = no
pext = no
avx2 = no

# Set architecture
ifeq ($(ARCH),x86-64-popc)
	arch = x86_64
	bits = 64
	prefetch = yes
	popcnt = yes
	sse = yes
	sse2 = yes
	ssse3 = yes
	sse41 = yes
endif

ifeq ($(ARCH),x86-64-avx2)
	arch = x86_64
	bits = 64
	prefetch = yes
	popcnt = yes
	sse = yes
	sse2 = yes
	ssse3 = yes
	sse41 = yes
	avx2 = yes
endif

ifeq ($(ARCH),x86-64-bmi2)
	arch = x86_64
	bits = 64
	prefetch = yes
	popcnt = yes
	sse = yes
	sse2 = yes
	ssse3 = yes
	sse41 = yes
	avx2 = yes
	pext = yes
endif

ifeq ($(ARCH),)
	arch = x86_64
	bits = 64
	prefetch = yes
	popcnt = yes
	sse = yes
	sse2 = yes
	ssse3 = yes
	sse41 = yes
	avx2 = yes
	pext = yes
endif

# Define INFO for additional messages on runtime
# ifeq ($(INFO),yes)
# 	CFLAGS =-DINFO
# endif

CXXFLAGS += -Wcast-qual -fno-exceptions
DEPENDFLAGS += -std=c++17

# Set compiler
ifeq ($(CXX),)
	CXX=g++
endif

ifeq ($(CXX),g++)
	CXX=g++
endif

# ifeq ($(CXX),clang++)
# 	CXX=clang++
# endif

# Set flags according to architecture options
ifeq ($(INFO),yes)
	CXXFLAGS += -DINFO=
endif

ifeq ($(optimize),yes)
	CXXFLAGS += -O3
endif

ifeq ($(bits),64)
	CXXFLAGS += -DIS_64_BIT
endif

ifeq ($(prefetch),yes)
	ifeq ($(sse),yes)
		CXXFLAGS += -msse
		DEPENDFLAGS += -msse
	endif
else
	CXXFLAGS += -DNO_PREFETCH
endif

ifeq ($(popcnt),yes)
	ifeq ($(bits),64)
		CXXFLAGS += -msse3 -mpopcnt -DUSE_POPCNT
	else
		CXXFLAGS += -mpopcnt -DUSE_POPCNT
	endif
endif

ifeq ($(avx2),yes)
	CXXFLAGS += -DUSE_AVX2
	CXXFLAGS += -mavx2
endif
ifeq ($(pext),yes)
	CXXFLAGS += -DUSE_PEXT
	CXXFLAGS += -mbmi -mbmi2
endif

CXXFLAGS += -flto
LDFLAGS += $(CXXFLAGS) # $(CFLAGS)

BUILD_DIR := ./build
SRC_DIR := ./chai

CPP = $(wildcard $(SRC_DIR)/*.cpp)

# All .o files go to build dir.
OBJ = $(CPP:%.cpp=$(BUILD_DIR)/%.o)
# Gcc/Clang will create these .d files containing dependencies.
DEP = $(OBJ:%.o=%.d)

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
	del /Q $(subst /,\,.\$(OBJ))
	del /Q $(subst /,\,.\$(DEP))

default:
	$(MAKE) help

build:
	$(MAKE) ARCH=$(ARCH) CXX=$(CXX) config
	$(MAKE) ARCH=$(ARCH) CXX=$(CXX) $(EXE)


help:
	@echo ""
	@echo "To compile chai, type: "
	@echo "make build [ARCH=arch] [INFO=yes|no]"
	@echo ""
	@echo "Supported targets:"
	@echo "build                   > Default build"
	@echo "clean                   > Clean up"
	@echo "help                    > Compiler options"
	@echo ""
	@echo "Supported architectures:"
	@echo "x86-64-popc             > x86 64-bit with popcnt support"
	@echo "x86-64-avx2             > x86 64-bit with avx2 support"
	@echo "x86-64-bmi2(default)    > x86 64-bit with bmi2 support"
	@echo ""
	@echo "Supported compilers:"
	@echo "g++ (default)
	@echo "clang++"
	@echo ""


config:
	@echo ""
	@echo "Settings:"
	@echo "debug: '$(debug)'"
	@echo "optimize: '$(optimize)'"
	@echo "arch: '$(arch)'"
	@echo "bits: '$(bits)'"
	@echo "prefetch: '$(prefetch)'"
	@echo "popcnt: '$(popcnt)'"
	@echo "sse: '$(sse)'"
	@echo "sse2: '$(sse2)'"
	@echo "ssse3: '$(ssse3)'"
	@echo "sse41: '$(sse41)'"
	@echo "avx2: '$(avx2)'"
	@echo "pext: '$(pext)'"	
	@echo ""
	@echo "Compiler:"
	@echo "CXX: $(CXX)"
	@echo "CXXFLAGS: $(CXXFLAGS)"
	@echo "DEPENDFLAGS: $(DEPENDFLAGS)"
	@echo ""


# Include all .d files
-include $(DEP)

.PHONY: clean build all default config
