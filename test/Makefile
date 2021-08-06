GTEST = C:/googletest/googletest/lib/libgtest.a
GTEST_PATH = C:/googletest/googletest/include

CXX = g++
CXXFLAGS = -std=c++17 -Wall -I h -I$(GTEST_PATH) -c
LXXFLAGS = -std=c++17 -I h -pthread
DEPENDFLAGS += -std=c++17

EXE = test.exe

CHAI_SRCS = attacks.cpp eval.cpp info.cpp mask.cpp move.cpp \
	board.cpp moveGenerator.cpp moveOrdering.cpp perft.cpp search.cpp \
	syzygy.cpp tt.cpp uci.cpp validate.cpp egtb/tbprobe.cpp egtb/tbchess.cpp

SRCS = $(addprefix ../chai/, $(CHAI_SRCS))

SRCS += testMain.cpp testSearch.cpp testPerft.cpp testSee.cpp

OBJ_DIR = ../chai/obj
OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SRCS)))

VPATH = ../chai:../chai/egtb:

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
