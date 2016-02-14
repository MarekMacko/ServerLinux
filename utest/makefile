LD_FLAGS = -L. -Wl,--no-as-needed -ftest-coverage -lstub  -fprofile-arcs -ldl -lgtest -lgmock -lgmock_main -pthread -lgcov
	

all: server libstub test
	
server:
	make -f ../build/makefile all

libstub:
	$(CC) -g -fPIC ../src/os/os.c -shared -o libstub.so

test:
	$(CXX) -g -I../src -I../src/os  mocks/os_mock.cpp server_test.cpp \
		../src/protocol.o ../src/acceptor_eh.o ../src/if_config.o \
		../src/port_configurator.o ../src/client_eh.o ../src/reactor.o \
		$(LD_FLAGS) -o server_test

run:
	./run.sh

report:
	mkdir html
	lcov --capture --directory ../src/ --output-file cov.info
	genhtml cov.info --output-directory html

clean:
	make -f ../build/makefile clean
	rm -f libstub.so server_test
	rm -f cov.info
	rm -rf html
	rm -f *.gcno
