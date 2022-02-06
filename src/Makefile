#######################################
# 1. META DATA
#######################################

VERSION = v3.4.0

EXE			= chai_$(VERSION)
EXE_RELEASE = $(EXE).exe
EXE_DEBUG   = $(EXE)_debug.exe
EXE_TEST    = test.exe

SRCS = main.cpp tt.cpp thread.cpp layers.cpp pawn.cpp transformer.cpp \
	board.cpp moveGenerator.cpp moveOrdering.cpp perft.cpp search.cpp \
	syzygy.cpp timeMan.cpp validate.cpp egtb/tbprobe.cpp bitboard.cpp \
	info.cpp nnue.cpp endgame.cpp attacks.cpp uci.cpp eval.cpp \


OBJ_RELEASE_DIR = ./obj_release
OBJS_RELEASE    = $(patsubst %.cpp, $(OBJ_RELEASE_DIR)/%.o, $(notdir $(SRCS)))

OBJ_DEBUG_DIR   = ./obj_debug
OBJS_DEBUG      = $(patsubst %.cpp, $(OBJ_DEBUG_DIR)/%.o, $(notdir $(SRCS)))

VPATH = egtb:nnue:


#######################################
# 2. Configure architecture & type
#######################################

# USER choices
assert   = n
debug    = n
nnue     = y
evalfile = n

# Default architecture enables all
prefetch = y
optimize = y
popcnt   = y
ssse3    = y
avx2     = y


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

ifeq ($(nnue),y)
	CXXFLAGS += -DUSE_NNUE
	ifneq ($(evalfile),n)
		CXXFLAGS += -DCUSTOM_EVALFILE=\"$(evalfile)\"
	endif
endif


####
#### 2.2 Set additional flags for respective build type.
ifeq ($(TYPE),release)
	assert = n
	debug  = n
endif

ifeq ($(TYPE),debug)
	assert   = y
	debug    = y
	optimize = n
endif

ifeq ($(assert),y)
	CXXFLAGS += -DASSERT
endif

ifeq ($(debug),y)
	CXXFLAGS += -DINFO
endif



####
#### 2.3 Set optimization flags as previously defined.
ifeq ($(optimize),y)
	CXXFLAGS += -O3
endif

ifeq ($(popcnt),y)
	CXXFLAGS += -mssse3 -mpopcnt
endif

ifeq ($(prefetch),y)
	CXXFLAGS += -DUSE_PREFETCH -msse
	DEPENDFLAGS += -msse
endif

ifeq ($(avx2),y)
	CXXFLAGS += -DUSE_AVX2 -mavx2
endif

ifeq ($(ssse3),y)
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

archive:
	$(MAKE) clean
	$(MAKE) release
	copy .\$(EXE_RELEASE) ..\archive\$(EXE_RELEASE)
	rename ..\archive\$(EXE_RELEASE) $(EXE)_1CPU.exe
	$(MAKE) clean
	$(MAKE) release threads=3
	copy .\$(EXE_RELEASE) ..\archive\$(EXE_RELEASE)
	rename ..\archive\$(EXE_RELEASE) $(EXE)_3CPU.exe
	$(MAKE) clean
	$(MAKE) release threads=4
	copy .\$(EXE_RELEASE) ..\archive\$(EXE_RELEASE)
	rename ..\archive\$(EXE_RELEASE) $(EXE)_4CPU.exe
	$(MAKE) clean
	$(MAKE) release evalfile="F:/SmartGit-Repositories/chai/nets/nn-fb50f1a2b1-20210705.nnue"
	copy .\$(EXE_RELEASE) ..\archive\$(EXE_RELEASE)
	rename ..\archive\$(EXE_RELEASE) $(EXE)_1CPU_nnue.exe
	$(MAKE) clean
	$(MAKE) release threads=3 evalfile="F:/SmartGit-Repositories/chai/nets/nn-fb50f1a2b1-20210705.nnue"
	copy .\$(EXE_RELEASE) ..\archive\$(EXE_RELEASE)
	rename ..\archive\$(EXE_RELEASE) $(EXE)_3CPU_nnue.exe
	$(MAKE) clean
	$(MAKE) release threads=4 evalfile="F:/SmartGit-Repositories/chai/nets/nn-fb50f1a2b1-20210705.nnue"
	copy .\$(EXE_RELEASE) ..\archive\$(EXE_RELEASE)
	rename ..\archive\$(EXE_RELEASE) $(EXE)_4CPU_nnue.exe

strip:
	$(MAKE) release
	$(MAKE) cleanObj

test:
	cd ../test & $(MAKE) TYPE=debug evalfile=$(evalfile)

clean:
	$(MAKE) cleanExe
	$(MAKE) cleanObj
	$(MAKE) cleanTest

cleanExe:
	del /Q $(EXE_DEBUG) 2>nul
	del /Q $(EXE_RELEASE) 2>nul
	del /Q $(EXE_TEST) 2>nul

cleanTest:
	del /Q .\obj_debug\ 2>nul
	del /Q $(EXE_TEST) 2>nul

cleanObj:
	del /Q .\obj_release\ 2>nul
	del /Q .\obj_debug\ 2>nul

help:
	$(info To build chai_$(VERSION), use the "release" target (optional parameters are indicated in brackets))
	$(info > make release [evalfile=path/to/nnue.bin] [avx2=y/n] [ssse3=y/n])
	@:

.depend:
	-@$(CXX) $(DEPENDFLAGS) -MM $(SRCS) > $@ 2> /dev/null

-include .depend

.PHONY: all test strip clean objclean help cleantest release debug