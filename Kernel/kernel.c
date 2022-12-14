// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <lib.h>
#include <moduleLoader.h>
#include <drivers/graphics.h>
#include <libs/memoryManager.h>
#include <libs/scheduler.h>
#include <drivers/RTC.h>
#include <drivers/keyboard.h>
#include <interrupts/time.h>
#include <interrupts/idtLoader.h>
#include <interrupts/interrupts.h>
#include <interrupts/syscalls.h>
#include <libs/processManager.h>
#include <libs/semaphore.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void *const shellAddress = (void *)0x400000;
static void *const heapAddress  = (void *)0x600000;

typedef int (*EntryPoint)();

extern void _hlt();
static void printLogo();

void clearBSS(void *bssAddress, uint64_t bssSize) {
	memset(bssAddress, 0, bssSize);
}

void * getStackBase() {
	return (void *)((uint64_t)&endOfKernel + PageSize * 8 // The size of the stack itself, 32KiB
					- sizeof(uint64_t)					  // Begin at the top of the stack
	);
}

void * initializeKernelBinary() {

	void *moduleAddresses[] = {shellAddress};

	loadModules(&endOfKernelBinary, moduleAddresses);

	clearBSS(&bss, &endOfKernel - &bss);

	load_idt();

    minit(heapAddress, HEAP_SIZE + HEAP_STRUCTURE_SIZE);

    initSemaphores();

	initGraphics();
	return getStackBase();
}

int init_shell() {
	char * argv[]= {"Shell", (void *) 0};
    newProcess((uint64_t)shellAddress, 0 , 1, 1, argv);
	return 1;

}

int main() {

	printLogo();
	gPrint("Presione enter para acceder a la consola");
	char c;
	do {
		// freno la ejecucion del programa hasta la siguiente interrupcion
		_hlt();
		// lo sigo haciendo hasta que la interrupccion sea debido a un enter
	} while ((c = getchar()) != '\n');
	gClear();


	_cli();
	startScheduler();
	init_shell();

	_sti();

	while (1);
	return 0;
}

#define LOGO_WIDTH	4
#define LOGO_HEIGHT	5
static void printLogo() {
	// https://patorjk.com/software/taag/#p=testall&f=Cosmike&t=BYTE
	gcolor C1 = {96, 60, 20};
	gcolor C2 = {156, 39, 6};
	gcolor C3 = {212, 91, 18};
	gcolor C4 = {243, 188, 46};
	gcolor BLACK = {0x00, 00, 00};
	gcolor colors[] = {C1, C2, C3, C4};
	char * logo[LOGO_HEIGHT][LOGO_WIDTH] = {
		{"      /\\/\\/\\/\\/\\    ", "/\\/\\    /\\/\\  ", "/\\/\\/\\/\\/\\/\\  ", "/\\/\\/\\/\\/\\/\\ \n"},
		{"     /\\/\\    /\\/\\  ", "/\\/\\    /\\/\\      ", "/\\/\\      ", "/\\            \n"},
		{"    /\\/\\/\\/\\/\\      ", "/\\/\\/\\/\\        ", "/\\/\\      ", "/\\/\\/\\/\\/\\     \n",},
		{"   /\\/\\    /\\/\\      ", "/\\/\\          ", "/\\/\\      ", "/\\/\\            \n"},
		{"  /\\/\\/\\/\\/\\        ", "/\\/\\          ", "/\\/\\      ", "/\\/\\/\\/\\/\\/\\     \n"}
	};

	for(int i = 0; i < LOGO_HEIGHT; i++) {
		for(int j = 0; j < LOGO_WIDTH; j++) {
			gPrintColor(logo[i][j], BLACK, colors[j]);

		}
	}
	gNewline();
	return;
}
