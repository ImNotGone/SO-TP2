include Makefile.inc

MM="BUDDY"

KERNEL=kernel.bin
KERNEL_ELF=$(KERNEL:.bin=.elf)
KERNEL_LD=kernel.ld

SOURCES=$(wildcard *.c */*.c)
OBJECTS=$(SOURCES:.c=.o)

SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)

LOADERSRC=loader.asm
LOADEROBJECT=$(LOADERSRC:.asm=.o)

STATICLIBS=

KERNEL_OBJECTS=$(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM)

all: $(KERNEL)
gdb: all $(KERNEL_ELF)

$(KERNEL): $(STATICLIBS) $(KERNEL_OBJECTS)
	$(LD) $(LDFLAGS) -T $(KERNEL_LD) $^ -o $@

$(KERNEL_ELF): $(STATICLIBS) $(KERNEL_OBJECTS)
	$(LD) $(LDFLAGS) -T $(KERNEL_LD) --oformat=elf64-x86-64 $^ -o $@

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -DMM_$(MM) -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT): $(LOADERSRC)
	$(ASM) $(ASMFLAGS) $^ -o $@

clean:
	rm -rf */*.o *.o *.bin *.elf

.PHONY: all gdb clean
