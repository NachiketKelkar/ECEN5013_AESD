
obj-m += list_sort.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

bbg: 
	make ARCH=arm CROSS_COMPILE=~/AESD/HW2/Problem3/buildroot/output/host/bin/arm-buildroot-linux-uclibcgnueabihf- -C ~/AESD/HW2/Problem3/buildroot/output/build/linux-4796173fc58688055a99a1cef19a839174067220 M=$(shell pwd) modules

cleanbbg:
	make -C ~/AESD/HW2/Problem3/buildroot/output/build/linux-4796173fc58688055a99a1cef19a839174067220 M=$(shell pwd) clean
