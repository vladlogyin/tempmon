
CXXFLAGS= -I. -g -std=c++14 -lpthread -Wall -lmenu -lncurses -ltinfo

all: tempmon

tempmon: tempmon.cpp tempmon.h tempui.cpp tempui.h
	$(CXX) $(CXXFLAGS) -o bin/tempmon tempmon.cpp tempui.cpp

clean:
	-rm -rf bin

lines:
	@echo "$(shell wc -l *.* | tail -n1) lines of code" 
