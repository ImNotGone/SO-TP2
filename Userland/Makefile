include Makefile.inc

all: shell
gdb: all shell_gdb

shell:
	cd shell; make all

shell_gdb:
	cd shell; make gdb

clean:
	cd shell; make clean
	rm -rf *.bin *.elf


.PHONY: shell all clean
