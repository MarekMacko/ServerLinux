GCC=gcc
SOURCES = ../src/
OUTPUT = ../bin/
OBJS = server acceptor_eh

all: $(OBJS)

server: $(SOURCES)server.c
	$(GCC) `pkg-config --cflags libconfig` $< -o $(OUTPUT)$@  `pkg-config --libs libconfig`

acceptor_eh: $(SOURCES)acceptor_eh.c
	$(GCC) -I ../include $< -o $(OUTPUT)$@
