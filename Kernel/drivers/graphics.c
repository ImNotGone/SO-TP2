// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <drivers/graphics.h>
#include <drivers/font.h>
#include <lib.h>

static gwindow window; // -> pantallas
static uint8_t currentWindow = 0;
static uint8_t cantWindows = 0;
static char buffer[64] = { 0 };

static const struct vbe_mode_info_structure * graphicsInfo = (struct vbe_mode_info_structure *) 0x5C00;
static uint8_t fontHeight;
static uint8_t fontWidth;
static uint8_t cursorActive = 0;
// BLACK
static gcolor DEFAULT_BACKGROUND = {0x00, 0x00, 0x00};
// WHITE
static gcolor DEFAULT_FOREGROUND = {0xFF,0xFF,0xFF};

static uint32_t uintToBase(uint64_t value, char * buff, uint32_t base);

#define COLORCHANELLS   3
#define CHAR_CURSOR     '_'

static uint8_t * getPixel(int x, int y) {
    return (graphicsInfo->framebuffer+COLORCHANELLS*(graphicsInfo->width*y+x));
}

static void drawPixel(int x, int y, gcolor color){
    uint8_t * pixel = getPixel(x, y);
    pixel[0] = color.B;
    pixel[1] = color.G;
    pixel[2] = color.R;
}

static void gUpdatePos() {
    window.cy += ((window.cx + 1) == window.width)?1:0;
    window.cx = (window.cx + 1)% window.width;
}

static void scrollUp(){
    void * dest = (void *)graphicsInfo->framebuffer;
    void * src = (void *)((uint64_t)dest+COLORCHANELLS*(graphicsInfo->width*fontHeight));

    uint64_t bytes = COLORCHANELLS*(graphicsInfo->width*(graphicsInfo->height - fontHeight));

    memcpy(dest, src, bytes);
    memset((void*)((uint64_t)dest+bytes), 0, COLORCHANELLS * graphicsInfo->width * fontHeight);

    window.cy--;
}

static void gSetCursor(gcolor color) {
    if(!cursorActive) return;
    gPutcharColor(CHAR_CURSOR, window.background, color);
    if(window.cx == 0){
        // subo una fila
        window.cy -= 1;
        // me paro en la ultima posicion de la fila
        window.cx = window.width-1;
        return;
    }
    window.cx = (window.cx-1) % window.width;
}

void gSetDefaultBackground(gcolor background) {
    window.background = background;
}
void gSetDefaultForeground(gcolor foreground) {
    window.foreground = foreground;
}

gcolor gGetDefaultBackground() {
    return window.background;
}
gcolor gGetDefaultForeground() {
    return window.foreground;
}


int8_t initGraphics() {
    cursorActive = 0;
    currentWindow = 0;
    cantWindows = 1;
    fontHeight = getFontHeight();
    fontWidth = getFontWidth();
    gwindow aux = {
        .sx = 0,
        .sy = 0,
        .cx = 0,
        .cy = 0,
        .height = graphicsInfo->height/fontHeight,
        .width  = graphicsInfo->width/fontWidth,
        .background = DEFAULT_BACKGROUND,
        .foreground = DEFAULT_FOREGROUND,
    };
    window = aux;
    gClear();
    cursorActive = 1;
    return 1;
}


void gClearAll() {
    for (int i = 0; i < graphicsInfo->height; i++) {
        for (int j = 0; j < graphicsInfo->width; j++)
            drawPixel(j, i, DEFAULT_BACKGROUND);
    }
}

void gPutcharColor(uint8_t c, gcolor background, gcolor foreground) {
    switch (c) {
        case '\b': gBackSpace(); return;
        case '\n': gNewline(); return;
    }

    if(window.cy == window.height)
        scrollUp();

    const uint8_t * bitmap = getcharFont(c);
    // current bit y
    uint16_t cby = (window.sy + window.cy)*fontHeight;
    // current bit x
    uint16_t cbx = (window.sx + window.cx)*fontWidth;
    uint16_t aux = cbx;
    uint8_t mask;
    for(int i = 0; i < fontHeight; i++, cby++) {
        mask = 0x80;
        cbx = aux;
        for(int j = 0; j < fontWidth; j++, mask >>= 1, cbx++) {
            drawPixel(cbx, cby, (bitmap[i] & mask)?foreground:background);
        }
    }

    gUpdatePos();
    cursorActive = 1;
}

void gPutchar(uint8_t c) {
    gPutcharColor(c, window.background, window.foreground);
}

void gPrintColor(const char * str, gcolor background, gcolor foreground) {
    for(int i = 0; str[i] != 0; i++) {
        gPutcharColor(str[i], background, foreground);
    }
}

void gPrint(const char * str) {
    gPrintColor(str, window.background, window.foreground);
}

void gNewline() {
    gSetCursor(window.background);
    window.cx = 0;
    window.cy += 1;
    gSetCursor(window.foreground);
}

void gPrintDec(uint64_t value)
{
	gPrintBase(value, 10);
}

void gPrintHex(uint64_t value)
{
	gPrintBase(value, 16);
}

void gPrintBin(uint64_t value)
{
	gPrintBase(value, 2);
}

void gPrintBase(uint64_t value, uint32_t base)
{
    uintToBase(value, buffer, base);
    gPrint(buffer);
}

void gClear()
{
    window.cx = 0;
    window.cy = 0;
    for(int i=0; i < window.height ; i++){
        for(int j=0; j < window.width ; j++){
            // current bit y
            uint16_t cby = (window.sy + window.cy)*fontHeight;
            // current bit x
            uint16_t cbx = (window.sx + window.cx)*fontWidth;
            uint16_t aux = cbx;
            for(int k = 0; k < fontHeight; k++, cby++) {
                cbx = aux;
                for(int l = 0; l < fontWidth; l++, cbx++) {
                    drawPixel(cbx, cby, window.background);
                }
            }
            gUpdatePos();
        }
    }
    window.cx = 0;
    window.cy = 0;
    cursorActive = 1;
	gSetCursor(window.foreground);
}


static uint32_t uintToBase(uint64_t value, char * buff, uint32_t base)
{
	char *p = buff;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buff.
	*p = 0;

	//Reverse string in buff.
	p1 = buff;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}

void gCursorBlink() {
    gcolor foreground[] = {window.background, window.foreground};
    static int select = 0;
    gSetCursor(foreground[select++]);
    select = select % 2;
}

static void doBackSpace() {
    if(window.cx == 0 && window.cy == 0){
        return;
    }

    // la columna es 0 -> la fila != 0
    if(window.cx == 0){
        // subo una fila
        window.cy -= 1;
        // me paro en la ultima posicion de la fila
        window.cx = window.width-1;
        // imprimo un "blank"
        gPutcharColor(' ', window.background, window.foreground);
        // me vuelvo a mover para atras
        // subo una fila
        window.cy -= 1;
        // me paro en la ultima posicion de la fila
        window.cx = window.width-1;
        return;
    }

    // la fila es 0 -> la columna != 0
    window.cx = (window.cx-1) % window.width;
    gPutcharColor(' ', window.background, window.foreground);
    window.cx = (window.cx-1) % window.width;
}

void gBackSpace() {
	gSetCursor(window.background);
    doBackSpace();
	gSetCursor(window.foreground);
}
