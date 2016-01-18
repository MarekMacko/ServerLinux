LD_FLAGS = -L. -Wl,--no-as-needed -ldl -lstub -lgtest -lgmock -lgmock_main -pthread -lgcov

all: libstub test run report
	
server:
	make -C ../server cov

libstub:
	$(CC) -fPIC ../src/os/os.c -shared -o libstub.so

test:
	$(CXX) -I../src -I../src/os mocks/os_mock.cpp server_test.cpp \
		../src/protocol.o ../src/acceptor_eh.o ../src/if_config.o \
		../src/port_configurator.o  ../src/client_eh.o ../src/reactor.o \
		$(LD_FLAGS) -o server_test

run: 
	./run.sh

report:
	mkdir html
	lgcov --capture --directory ../server --output-file cov.info
	genhtml cov.info --output-directory html

clean:
	make -C ../server clean
	rm -f libstub.so server_test
	rm -f cov.info
	rm -rf html
