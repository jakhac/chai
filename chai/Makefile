#######################################
# 1. META DATA
#######################################

VERSION = v2.6.4

EXE = chai_$(VERSION).exe

SRCS = main.cpp tt.cpp attacks.cpp eval.cpp info.cpp mask.cpp move.cpp \
	board.cpp moveGenerator.cpp moveOrdering.cpp perft.cpp search.cpp \
	syzygy.cpp uci.cpp validate.cpp egtb/tbprobe.cpp egtb/tbchess.cpp

OBJ_DIR = ./obj
OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SRCS)))

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


ifeq ($(TYPE),)
	TYPE = release
endif
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
	test=yes

	LDFLAGS += -fsanitize=address
	CXXFLAGS += -fsanitize=address
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

DEPENDFLAGS += -std=c++17
LDFLAGS += -m$(bits)


#######################################
# 3. Compiler and link to target
#######################################

# Use GNU compiler
CXX = g++

$(EXE): $(OBJS)
	+$(CXX) -o $@ $(OBJS) $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cpp
	g++ $(CXXFLAGS) -c -o $@ $<


#######################################
# 4. Public targets
#######################################

debug:
	$(MAKE) $(EXE) TYPE=debug .depend

release: 
	$(MAKE) $(EXE) TYPE=release .depend

# strip

test:
	cd .. & cd test & $(MAKE) TYPE=debug

clean:
	$(MAKE) objclean
	del /Q $(EXE) 2>nul

objclean:
	del /Q .\obj\ 2>nul

help:
	@echo "Build chai_$(VERSION) with "make [target]"
	@echo "Possible targets are:"
	@echo "debug     > build with asserts and debug information"
	@echo "release   > build fastest version of engine"
	@echo "test      > copmile engine files and build test program in root/test"

.depend:
	-@$(CXX) $(DEPENDFLAGS) -MM $(SRCS) > $@ 2> /dev/null

-include .depend

.PHONY: all test clean objclean help