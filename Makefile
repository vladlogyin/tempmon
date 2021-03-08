
#CXXFLAGS= -I. -g -std=c++14 -lpthread -Wall -lmenu -lncurses -ltinfo
CXXFLAGS= -I. -std=c++14 -Wall
CXXFLAGS+= -g

LDFLAGS= -lmenu -lncurses -ltinfo -lpthread -lstdc++
LDFLAGS+= -g

all: tempmon

tempmon: tempmon.o tempui.o

clean:
	-rm *.o

lines:
	@echo "$(shell wc -l *.* | tail -n1) lines of code" 
