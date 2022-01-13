GTEST = C:/googletest/googletest/lib/libgtest.a
GTEST_PATH = C:/googletest/googletest/include

CXX = g++
CXXFLAGS = -std=c++17 -Wall -I h -I$(GTEST_PATH) -c
LXXFLAGS = -std=c++17 -I h -pthread
LXXFLAGS += -m64 -Wl,--stack,4194304
DEPENDFLAGS += -std=c++17 -fexceptions

CXXFLAGS += -DINFO= -DASSERT= -DTESTING=
CXXFLAGS += -Wall -Wcast-qual -fno-exceptions -std=c++17 -m64
CXXFLAGS += -pedantic -Wextra -Wshadow
CXXFLAGS += -Wl,--stack,4194304 -fexceptions


EXE = ../src/test.exe

CHAI_SRCS = main.cpp tt.cpp thread.cpp eval.cpp pawn.cpp mask.cpp move.cpp \
	board.cpp moveGenerator.cpp moveOrdering.cpp perft.cpp search.cpp \
	syzygy.cpp uci.cpp validate.cpp egtb/tbprobe.cpp egtb/tbchess.cpp \
	attacks.cpp timeMan.cpp info.cpp transformer.cpp nnue.cpp layers.cpp

SRCS = $(addprefix ../chai/, $(CHAI_SRCS))

SRCS += testMain.cpp testSearch.cpp testPerft.cpp testSee.cpp testEval.cpp

OBJ_DIR = ../src/obj_debug
OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SRCS)))

VPATH = ../src:../src/egtb:../src/nnue:

$(EXE): $(OBJS)
	$(CXX) $(LXXFLAGS) -o $(EXE) $(OBJS) $(GTEST)

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<


### Public targets
all:
	$(MAKE) $(EXE) .depend

clean:
	del /Q $(EXE) 2>nul
	del /Q .\obj\ 2>nul


.depend:
	-@$(CXX) $(DEPENDFLAGS) -MM $(SRCS) > $@ 2> /dev/null

-include .depend
