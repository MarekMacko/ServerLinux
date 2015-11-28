GCC = gcc
CFLAGS = -Wall 
SOURCES = ../src/
OUTPUT = ../bin/
OBJS = main reactor

all: $(OBJS)

main: $(SOURCES)main.c 
	$(GCC) `pkg-config --cflags libconfig` $< -o $(OUTPUT)$@  `pkg-config --libs libconfig`

reactor.o: $(SOURCES)reactor.c
	$(GCC) -w -c $(CFLAGS) $<  


