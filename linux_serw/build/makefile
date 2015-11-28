CC=gcc
CFLAGS=-Wall -g
LDFLAGS=
SOURCES=../src/main.c\
	../src/reactor.c\
	../src/reader.c
NAME=serwer
LIB_NAME=libserwer.so

OBJECTS=$(SOURCES:.c=.o)

all:$(NAME) $(LIB_NAME)

%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

$(LIB_NAME):$(OBJECTS)
	$(CC) $(SOURCES) -shared -fPIC -o $(LIB_NAME)

$(NAME):$(LIB_NAME)
	$(CC) `pkg-config --cflags libconfig` $(OBJECTS) -I . -L . -l$(NAME) -o $(NAME) `pkg-config --libs libconfig`

clean:
	rm -f $(OBJECTS)
	rm -f $(NAME)
	rm -f $(LIB_NAME)

clean-o:
	rm -f $(OBJECTS)
