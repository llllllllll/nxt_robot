# Joe Jevnik
# 10.13.2013

CC=gcc
CFLAGS=-c -std=gnu99 -O2
LDFLAGS=
SOURCES=main.c screen/scr.c robot_control/remote.c robot_control/left.c robot_control/right.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=test

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ -L. -lC_NXT -lncurses -lpthread -lbluetooth

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(EXECUTABLE) $(OBJECTS)
