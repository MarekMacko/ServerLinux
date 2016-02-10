obj-m += ../src/module.o
#ccflags-y += -g -I../src/

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

install:
	insmod ../src/module.ko

uninstall:
	rmmod module

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f ../src/module.ko
	rm -f ../src/module.mod.c
	rm -f ../src/module.mod.o
	rm -f ../src/module.o
	rm -f ../src/.module.ko.cmd
	rm -f ../src/.module.mod.o.cmd
	rm -f ../src/.module.o.cmd
	rm -f ../src/.tmp_module.dwo
	rm -f ../src/*.dwo