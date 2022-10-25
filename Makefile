include Makefile.inc

all: bootloader kernel userland image
gdb: all kernel_elf userland_elf

bootloader:
	cd Bootloader; make all

kernel:
	cd Kernel; make all MM=$(MM)

kernel_elf:
	cd Kernel; make gdb MM=$(MM)

userland:
	cd Userland; make all

userland_elf:
	cd Userland; make gdb

image: kernel bootloader userland
	cd Image; make all

$(CREDENTIALS):
	    pvs-studio-analyzer credentials "PVS-Studio Free" "FREE-FREE-FREE-FREE"

pvs: $(CREDENTIALS)
	make clean
	pvs-studio-analyzer trace -- make
	pvs-studio-analyzer analyze
	plog-converter -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t tasklist -o report.tasks PVS-Studio.log

cpp-check:
	cppcheck --quiet --enable=all --force --inconclusive --suppress=missingIncludeSystem .

clean:
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean
	rm -rf $(PVS_OUTPUT)

.PHONY: bootloader image pvs cpp-check kernel kernel_elf userland userland_elf all gdb clean


