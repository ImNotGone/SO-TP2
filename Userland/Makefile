include Makefile.inc

SAMPLE_DATA=0001-sampleDataModule.bin

all: sampleCodeModule sampleDataModule
gdb: all sampleCodeModule_gdb

sampleCodeModule:
	cd SampleCodeModule; make all

sampleCodeModule_gdb:
	cd SampleCodeModule; make gdb


sampleDataModule:
	printf "This is sample data." >> $(SAMPLE_DATA) && dd if=/dev/zero bs=1 count=1 >> $(SAMPLE_DATA)

clean:
	cd SampleCodeModule; make clean
	rm -rf *.bin *.elf


.PHONY: sampleCodeModule all clean
