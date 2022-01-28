#######################################
# 1. META DATA
#######################################

VERSION = v3.2.0

EXE_RELEASE = chai_$(VERSION)_release.exe
EXE_DEBUG   = chai_$(VERSION)_debug.exe
EXE_TEST    = test.exe

SRCS = main.cpp tt.cpp thread.cpp eval.cpp pawn.cpp mask.cpp move.cpp \
	board.cpp moveGenerator.cpp moveOrdering.cpp perft.cpp search.cpp \
	syzygy.cpp uci.cpp validate.cpp egtb/tbprobe.cpp egtb/tbchess.cpp \
	timeMan.cpp info.cpp nnue.cpp endgame.cpp attacks.cpp layers.cpp  \
	transformer.cpp

OBJ_RELEASE_DIR = ./obj_release
OBJS_RELEASE = $(patsubst %.cpp, $(OBJ_RELEASE_DIR)/%.o, $(notdir $(SRCS)))

OBJ_DEBUG_DIR = ./obj_debug
OBJS_DEBUG = $(patsubst %.cpp, $(OBJ_DEBUG_DIR)/%.o, $(notdir $(SRCS)))

VPATH = egtb:nnue:


#######################################
# 2. Configure architecture & type
#######################################

# Build type
assert   = no
debug    = no
nnue     = yes
evalfile = nn-fb50f1a2b1-20210705.nnue

# Default architecture enables all
prefetch = yes
optimize = yes
popcnt   = yes
ssse3    = yes
avx2     = yes

####
#### 2.1 Define optional constants if passed by user.
threads = 1
hashMb  = 256

ifneq ($(threads),1)
	CXXFLAGS += -DCUSTOM_THREADS=$(threads)
endif

ifneq ($(hashMb),256)
	CXXFLAGS += -DCUSTOM_HASHSIZE=$(hashMb)
endif

ifneq ($(evalfile),none)
	CXXFLAGS += -DCUSTOM_EVALFILE=\"../nets/$(evalfile)\"
endif

####
#### 2.2 Set additional flags for respective build type.
ifeq ($(TYPE),release)
	assert = no
	debug  = no
endif

ifeq ($(TYPE),debug)
	assert   = yes
	debug    = yes
	optimize = no
endif

ifeq ($(assert),yes)
	CXXFLAGS += -DASSERT
endif

ifeq ($(debug),yes)
	CXXFLAGS += -DINFO
endif

ifeq ($(nnue),yes)
	CXXFLAGS += -DUSE_NNUE
endif



####
#### 2.3 Set optimization flags as previously defined.
ifeq ($(optimize),yes)
	CXXFLAGS += -O3
endif

ifeq ($(popcnt),yes)
	CXXFLAGS += -mssse3 -mpopcnt
endif

ifeq ($(prefetch),yes)
	CXXFLAGS += -DUSE_PREFETCH -msse
	DEPENDFLAGS += -msse
endif

ifeq ($(avx2),yes)
	CXXFLAGS += -DUSE_AVX2 -mavx2
endif

ifeq ($(ssse3),yes)
	CXXFLAGS += -DUSE_SSSE3 -mssse3 -msse2
endif


####
#### 2.4 Additional flags for code quality.
CXXFLAGS += -Wall -Wcast-qual -fno-exceptions -std=c++17 -m64
CXXFLAGS += -Wl,--stack,4194304 -fexceptions
CXXFLAGS += -pedantic -Wextra -Wshadow
CXXFLAGS += -DVERSION=$(VERSION)

DEPENDFLAGS += -std=c++17 -fexceptions

LDFLAGS += -m64 -Wl,--stack,4194304


#######################################
# 3. Compile and link to target
#######################################

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
	cd ../test & $(MAKE) TYPE=debug evalfile=$(evalfile)

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
	@echo "make clean     > Remove all binaries and object-files."

.depend:
	-@$(CXX) $(DEPENDFLAGS) -MM $(SRCS) > $@ 2> /dev/null

-include .depend

.PHONY: all test strip clean objclean help cleantest release debug