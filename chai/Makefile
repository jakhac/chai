### Meta data
VERSION = v2.6.4

### Exe name
EXE = chai_$(VERSION).exe

### Source and object files
SRCS = attacks.cpp defs.cpp eval.cpp info.cpp main.cpp mask.cpp move.cpp \
	board.cpp moveGenerator.cpp moveOrdering.cpp perft.cpp search.cpp \
	syzygy.cpp tt.cpp uci.cpp validate.cpp egtb/tbprobe.cpp egtb/tbchess.cpp

SRCS_TEST = ../test/pch.cpp ../test/testMoves.cpp ../test/testCheckers.cpp ../test/testSee.cpp \
	../test/testSearch.cpp ../test/testPerft.cpp ../test/testZugzwang.cpp

OBJ_DIR = ./obj

OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SRCS)))

VPATH = egtb:../test

### Options for custom build
assert=no
debug=no
info=yes
test=no

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

CXXFLAGS += -Wcast-qual -fno-exceptions
DEPENDFLAGS += -std=c++17

# Set flags according to optimzization settings
ifeq ($(info),yes)
	CXXFLAGS += -DINFO
endif

ifeq ($(assert),yes)
	CXXFLAGS += -DASSERT=
endif

ifeq ($(test),yes)
	CXXFLAGS += -DTESTING=
endif

# ifeq ($(DEBUG),yes)
# 	CXXFLAGS+= -DINFO=
# endif

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

### Compiler
CXX = g++

### Standard flags
CXXFLAGS += -Wall -Wcast-qual -fno-exceptions -std=c++17 -m$(bits)
DEPENDFLAGS += -std=c++17
LDFLAGS += -m$(bits)

### Warning flags
CXXFLAGS += -pedantic -Wextra -Wshadow

$(EXE): $(OBJS)
	+$(CXX) -o $@ $(OBJS) $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cpp
	g++ $(CXXFLAGS) -c -o $@ $<


### Public targets

chai:
	$(MAKE) $(EXE) .depend

clean:
	del /Q .\obj\ 2>nul
	del /Q $(EXE) 2>nul

# release: 
# 	info = no
# 	debug = no
# 	assert = no
# 	test = no
# 	$(MAKE) $(EXE) .depend

# debug: 
# 	info = yes
# 	debug = yes
# 	assert = yes
# 	test = no
# 	$(MAKE) $(EXE) .depend

#test

.depend:
	-@$(CXX) $(DEPENDFLAGS) -MM $(SRCS) > $@ 2> /dev/null

-include .depend

.PHONY: help build objclean