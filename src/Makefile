#######################################
# 1. META DATA
#######################################

UNAME = $(shell uname)

VERSION = v3.4.1
EXE		= chai_$(VERSION)


ifeq ($(comp),mingw)
	EXE_RELEASE = $(EXE).exe
	EXE_DEBUG   = $(EXE)_debug.exe
	EXE_TEST    = bench.exe
else
	EXE_RELEASE = $(EXE)
	EXE_DEBUG   = $(EXE)_debug
	EXE_TEST    = bench
endif


ENG_SRC = uci.cpp tt.cpp thread.cpp layers.cpp pawn.cpp attacks.cpp \
	board.cpp moveGenerator.cpp moveOrdering.cpp perft.cpp search.cpp \
	syzygy.cpp timeMan.cpp validate.cpp egtb/tbprobe.cpp bitboard.cpp \
	info.cpp nnue.cpp endgame.cpp transformer.cpp eval.cpp \

TEST_SRC = ../test/testMain.cpp ../test/testSearch.cpp \
	../test/testPerft.cpp ../test/testSee.cpp ../test/testEval.cpp \
	../test/testPosition.cpp

TEST_SRCS = $(ENG_SRC) $(TEST_SRC)
CHAI_SRCS = $(ENG_SRC) main.cpp
SRCS 	  = $(TEST_SRCS) $(CHAI_SRCS)

OBJ_RELEASE_DIR = ./obj_release
OBJ_DEBUG_DIR   = ./obj_debug

OBJS_RELEASE    = $(patsubst %.cpp, $(OBJ_RELEASE_DIR)/%.o, $(notdir $(CHAI_SRCS)))
OBJS_DEBUG      = $(patsubst %.cpp, $(OBJ_DEBUG_DIR)/%.o, $(notdir $(CHAI_SRCS)))
OBJS_TEST       = $(patsubst %.cpp, $(OBJ_DEBUG_DIR)/%.o, $(notdir $(TEST_SRCS)))

VPATH = egtb:nnue:../test:

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

LDFLAGS   = -m64 

DEPENDFLAGS += -std=c++17 -fexceptions


#######################################
# 3. Compile and link to target
#######################################

ifeq ($(comp),)
	comp=gcc
endif

ifeq ($(comp),gcc)
	COMP=gcc
	CXX=g++

	LDFLAGS += -Wl,-z,stack-size=4194304 -pthread
endif

ifeq ($(comp),mingw)
	COMP=mingw

    ifeq ($(UNAME),Linux)
        CXX=x86_64-w64-mingw32-c++
    else
        CXX=g++
    endif

	LDFLAGS += -Wl,--stack,4194304
	LDFLAGS += -static
endif


### Compile to object files

$(OBJ_DEBUG_DIR)/%.o: %.cpp
    +$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_RELEASE_DIR)/%.o: %.cpp
    +$(CXX) $(CXXFLAGS) -c -o $@ $<


### Link to executables

$(EXE_DEBUG): $(OBJS_DEBUG)
	+$(CXX) -o $@ $(OBJS_DEBUG) $(LDFLAGS)


$(EXE_RELEASE): $(OBJS_RELEASE)
	+$(CXX) -o $@ $(OBJS_RELEASE) $(LDFLAGS)


$(EXE_TEST): $(OBJS_TEST)
	+$(CXX) -o $@ $(OBJS_TEST) $(LDFLAGS) -lgtest -lgtest_main -lpthread


#######################################
# 4. Public targets
#######################################

.DEFAULT_GOAL=help

debug:
    mkdir -p ./obj_debug
    $(MAKE) $(EXE_DEBUG) TYPE=debug .depend

release:
    mkdir -p ./obj_release
    $(MAKE) $(EXE_RELEASE) TYPE=release .depend

test:
	mkdir -p ./obj_debug
	$(MAKE) $(EXE_TEST) TYPE=debug .depend

clean:
	rm -f $(EXE_DEBUG) $(EXE_RELEASE) $(EXE_TEST) ./obj_debug/*.o ./obj_release/*.o

help:
    $(info To build chai, use the "release" target:)
    $(info > make release [COMP=comp] [evalfile=path/to/nnue.bin] [avx2=y/n] [ssse3=y/n])

archive:
    for number in 1 3 4 ; do \
        $(MAKE) clean ; \
        $(MAKE) release threads=$$number ; \
        mv ./$(EXE_RELEASE) ../archive/$(EXE)_"$$number"CPU.exe ; \
    done

.depend:
    -@$(CXX) $(DEPENDFLAGS) -MM $(SRCS) > $@ 2> /dev/null

-include .depend

.PHONY: all test strip clean objclean help cleantest release debug
