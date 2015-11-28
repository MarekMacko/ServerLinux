GCC = gcc
CFLAGS = -Wall 
SOURCES = ../src/
OUTPUT = ../bin/
OBJS = server reactor

all: $(OBJS)

server: $(SOURCES)server.c 
	$(GCC) `pkg-config --cflags libconfig` $< -o $(OUTPUT)$@  `pkg-config --libs libconfig`

reactor.o: $(SOURCES)reactor.c
	$(GCC) -w -c $(CFLAGS) $<  


