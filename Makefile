CXXFLAGS= -I. -std=c++14 -Wall
CXXFLAGS+= -g
CXXFLAGS+= $(shell pkgconf gtkmm-3.0 glibmm-2.4 --cflags)
#UI selection
#CXXFLAGS+= -DTEMPMON_CLI
CXXFLAGS+= -DTEMPMON_GTK

LDFLAGS= -lmenu -lncurses -ltinfo -lpthread -lstdc++ $(shell pkgconf gtkmm-3.0 glibmm-2.4 --libs)
LDFLAGS+= -g

all: tempmon

tempmon: tempmon.o client.o tempmon-cli.o tempmon-gtk.o

clean:
	-rm *.o tempmon

lines:
	@echo "$(shell wc -l *.cpp *.h | tail -n1) lines of code" 
