#######################################
# 1. META DATA
#######################################

UNAME = $(shell uname)

VERSION = v3.4.1
EXE		= chai_$(VERSION)


ifeq ($(COMP),mingw)
	EXE_RELEASE = $(EXE).exe
	EXE_DEBUG   = $(EXE)_debug.exe
	EXE_TEST    = test.exe
else
	EXE_RELEASE = $(EXE)
	EXE_DEBUG   = $(EXE)_debug
	EXE_TEST    = test
endif


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


#######################################
# 3. Compile and link to target
#######################################

ifeq ($(COMP),)
	COMP=gcc
endif

ifeq ($(COMP),gcc)
	comp=gcc
	CXX=g++

	LDFLAGS += -m64 -Wl,-z,stack-size=4194304 -lpthread
endif

ifeq ($(COMP),mingw)
	comp=mingw

	ifeq ($(UNAME),Linux)
		CXX=x86_64-w64-mingw32-c++
	else
		CXX=g++
	endif

	LDFLAGS += -m64 -Wl,--stack,4194304
	LDFLAGS += -static
endif


$(EXE_DEBUG): $(OBJS_DEBUG)
	+$(CXX) -o $@ $(OBJS_DEBUG) $(LDFLAGS)

$(OBJ_DEBUG_DIR)/%.o: %.cpp
	+$(CXX) $(CXXFLAGS) -c -o $@ $<

$(EXE_RELEASE): $(OBJS_RELEASE)
	+$(CXX) -o $@ $(OBJS_RELEASE) $(LDFLAGS)

$(OBJ_RELEASE_DIR)/%.o: %.cpp
	+$(CXX) $(CXXFLAGS) -c -o $@ $<


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
	cd ../test && $(MAKE) TYPE=debug evalfile=$(evalfile) cxx=$(CXX)
	mv ../test/$(EXE_TEST) .

clean:
	@rm -f $(EXE_DEBUG) $(EXE_RELEASE) $(EXE_TEST) ./obj_debug/*.o ./obj_release/*.o

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
