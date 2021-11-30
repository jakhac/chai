#######################################
# 1. META DATA
#######################################

VERSION = v2.6.6

EXE_RELEASE = chai_$(VERSION)_release.exe
EXE_DEBUG = chai_$(VERSION)_debug.exe
EXE_TEST = test.exe

SRCS = main.cpp tt.cpp attacks.cpp eval.cpp info.cpp mask.cpp move.cpp \
	board.cpp moveGenerator.cpp moveOrdering.cpp perft.cpp search.cpp \
	syzygy.cpp uci.cpp validate.cpp egtb/tbprobe.cpp egtb/tbchess.cpp \
	thread.cpp timeMan.cpp

OBJ_RELEASE_DIR = ./obj_release
OBJS_RELEASE = $(patsubst %.cpp, $(OBJ_RELEASE_DIR)/%.o, $(notdir $(SRCS)))

OBJ_DEBUG_DIR = ./obj_debug
OBJS_DEBUG = $(patsubst %.cpp, $(OBJ_DEBUG_DIR)/%.o, $(notdir $(SRCS)))

VPATH = egtb:


#######################################
# 2. Configure architecture & type
#######################################

# build type
assert=no
debug=no
info=yes
test=no

# architecture
optimize=yes
bits=64
prefetch=yes
popcnt=yes
sse=yes
sse2=yes
ssse3=yes
sse41=yes
avx2=yes
pext=yes
bits=64

# custom build configs
threads=0
hashMb=265


ifeq ($(TYPE),release)
	assert=no
	debug=no
	info=no
	test=no
endif
ifeq ($(TYPE),debug)
	assert=yes
	debug=yes
	info=yes
	test=no

	optimize=no
endif


ifneq ($(threads),0)
	CXXFLAGS += -DCUSTOM_THREADS=$(threads)
endif
ifneq ($(hashMb),265)
	CXXFLAGS += -DCUSTOM_HASHSIZE=$(hashMb)
endif


ifeq ($(info),yes)
	CXXFLAGS += -DINFO=
endif

ifeq ($(assert),yes)
	CXXFLAGS += -DASSERT=
endif

ifeq ($(test),yes)
	CXXFLAGS += -DTESTING=
endif

ifeq ($(DEBUG),yes)
	CXXFLAGS+= -DINFO=
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


# Always add standard flags
CXXFLAGS += -Wall -Wcast-qual -fno-exceptions -std=c++17 -m$(bits)

# Warning flags for cleaner code
CXXFLAGS += -pedantic -Wextra -Wshadow

# Add version flag for info in target exe
CXXFLAGS += -DVERSION=$(VERSION)

# Set 4MB stack size
CXXFLAGS += -Wl,--stack,4194304 -fexceptions

DEPENDFLAGS += -std=c++17 -fexceptions
LDFLAGS += -m$(bits) -Wl,--stack,4194304


#######################################
# 3. Compiler and link to target
#######################################

# Use GNU compiler
CXX = g++

$(EXE_DEBUG): $(OBJS_DEBUG)
	+$(CXX) -o $@ $(OBJS_DEBUG) $(LDFLAGS)

$(OBJ_DEBUG_DIR)/%.o: %.cpp
	g++ $(CXXFLAGS) -c -o $@ $<

$(EXE_RELEASE): $(OBJS_RELEASE)
	+$(CXX) -o $@ $(OBJS_RELEASE) $(LDFLAGS)

$(OBJ_RELEASE_DIR)/%.o: %.cpp
	g++ $(CXXFLAGS) -c -o $@ $<



#######################################
# 4. Public targets
#######################################

.DEFAULT_GOAL=help

debug:
	if not exist ".\obj_debug" mkdir ".\obj_debug"
	$(MAKE) $(EXE_DEBUG) TYPE=debug .depend

release:
	if not exist ".\obj_release" mkdir ".\obj_release"
	$(MAKE) $(EXE_RELEASE) TYPE=release .depend

strip:
	$(MAKE) release
	$(MAKE) objClean

test:
	cd ../test & $(MAKE) TYPE=debug

clean:
	$(MAKE) cleanExe
	$(MAKE) objClean
	$(MAKE) cleanTest

cleanExe:
	del /Q $(EXE_DEBUG) 2>nul
	del /Q $(EXE_RELEASE) 2>nul
	del /Q $(EXE_TEST) 2>nul

cleanTest:
	del /Q .\obj_debug\ 2>nul
	del /Q $(EXE_TEST) 2>nul

objClean:
	del /Q .\obj_release\ 2>nul
	del /Q .\obj_debug\ 2>nul

help:
	@echo "Build chai_$(VERSION) with "make target [threads=#threads] [hashMb=#hashMb]"
	@echo "Possible targets are:"
	@echo "make debug     > Build includes assertions, extra checks and information."
	@echo "make release   > Build optimized on performance and speed."
	@echo "make test      > Build test-suite."

.depend:
	-@$(CXX) $(DEPENDFLAGS) -MM $(SRCS) > $@ 2> /dev/null

-include .depend

.PHONY: all test strip clean objclean help cleantest release debug