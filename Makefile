
#CXXFLAGS= -I. -g -std=c++14 -lpthread -Wall -lmenu -lncurses -ltinfo
CXXFLAGS= -I. -std=c++14 -Wall
CXXFLAGS+= -g
#UI selection
CXXFLAGS+= -DTEMPMON_CLI

LDFLAGS= -lmenu -lncurses -ltinfo -lpthread -lstdc++
LDFLAGS+= -g

all: tempmon

tempmon: tempmon.o tempmon-cli.o

clean:
	-rm *.o tempmon

lines:
	@echo "$(shell wc -l *.* | tail -n1) lines of code" 
