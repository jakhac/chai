EXE = test

LXXFLAGS  = -std=c++17 -I h 
LXXFLAGS += -m64 -Wl,-z,stack-size=4194304
LXXFLAGS += -lgtest -lgtest_main -lpthread

CXXFLAGS += -DINFO= -DASSERT= -DTESTING=
CXXFLAGS += -Wall -Wcast-qual -fno-exceptions -std=c++17
CXXFLAGS += -pedantic -Wextra -Wshadow
CXXFLAGS += -m64 -Wl,-z,stack-size=4194304 -fexceptions

CXXFLAGS += -DUSE_PREFETCH -msse
CXXFLAGS += -DUSE_AVX2 -mavx2
CXXFLAGS += -DUSE_SSSE3 -mssse3 -msse2
CXXFLAGS += -mssse3 -mpopcnt

DEPENDFLAGS += -std=c++17 -fexceptions
DEPENDFLAGS += -msse

nnue     = y
evalfile = n
ifneq ($(evalfile),n)
	CXXFLAGS += -DCUSTOM_EVALFILE=\"$(evalfile)\"
endif
ifeq ($(nnue),y)
	CXXFLAGS += -DUSE_NNUE
endif


CHAI_SRCS = uci.cpp tt.cpp thread.cpp layers.cpp pawn.cpp attacks.cpp \
	board.cpp moveGenerator.cpp moveOrdering.cpp perft.cpp search.cpp \
	syzygy.cpp timeMan.cpp validate.cpp egtb/tbprobe.cpp bitboard.cpp \
	info.cpp nnue.cpp endgame.cpp transformer.cpp eval.cpp \

SRCS = $(addprefix ../chai/, $(CHAI_SRCS))

SRCS += testMain.cpp testSearch.cpp testPerft.cpp \
	testSee.cpp testEval.cpp testPosition.cpp

OBJ_DIR = ../src/obj_debug
OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SRCS)))

VPATH = ../src:../src/egtb:../src/nnue:

$(EXE): $(OBJS)
	$(cxx) -o $(EXE) $(OBJS) $(LXXFLAGS)

$(OBJ_DIR)/%.o: %.cpp
	$(cxx) $(CXXFLAGS) -c -o $@ $<


### Public targets
all:
	$(MAKE) $(EXE) .depend


.depend:
	-@$(CXX) $(DEPENDFLAGS) -MM $(SRCS) > $@ 2> /dev/null

-include .depend
