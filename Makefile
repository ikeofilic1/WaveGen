obj-m += wavegen_driver.o
DIR=/lib/modules/$(shell uname -r)/build

build:
	gcc wavegen_ip.c wavegen.c -Wall -Wextra -o wavegen

kernel:
	make -C $(DIR) M=$(shell pwd) modules

clean:
	make -C $(DIR) M=$(shell pwd) clean
