#include "display.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"

Graphics_Context g_sContext;

void InitDisplay() {
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(&g_sContext,
                         &g_sCrystalfontz128x128,
                         &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, FOREGROUNDCOLOR);
    Graphics_setBackgroundColor(&g_sContext, BACKGROUNDCOLOR);
    Graphics_setFont(&g_sContext, &g_sFontCmtt16);
    Graphics_clearDisplay(&g_sContext);
}

void DrawTime(unsigned minutes, unsigned seconds) {
    char buf[5];
    buf[4] = (seconds % 10) + '0';
    buf[3] = (seconds / 10) % 10 + '0';
    buf[2] = ':';
    buf[1] = (minutes % 10) + '0';
    buf[0] = (minutes / 10) % 10 + '0';
    Graphics_drawStringCentered(&g_sContext, (int8_t *) buf, 5, 64, 120, OPAQUE_TEXT);
}

void DrawMessage(char *s, uint32_t color) {
    Graphics_setForegroundColor(&g_sContext, color);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) s, -1, 64, 8, OPAQUE_TEXT);
}

void DrawScore(int computerscore, int humanscore, uint32_t color) {
    char buf[16] = "Score XX - XX ";
    buf[ 7] =  (char) ('0' + ( computerscore % 10));
    buf[ 6] =  (char) ('0' + ((computerscore/10) % 10));
    buf[12] =  (char) ('0' + ( humanscore % 10));
    buf[11] =  (char) ('0' + ((humanscore/10) % 10));
    Graphics_setForegroundColor(&g_sContext, color);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) buf, -1, 64, 8, OPAQUE_TEXT);
}

void DrawBoard(tcellstate map[9]) {
    int8_t *symbolMap[3] = {" ", "X", "O"};

    Graphics_setForegroundColor(&g_sContext, LINECOLOR);
    Graphics_drawLineH(&g_sContext, 19,        109,    49);
    Graphics_drawLineH(&g_sContext, 19,        109,    79);
    Graphics_drawLineV(&g_sContext, 49,        19,     109);
    Graphics_drawLineV(&g_sContext, 79,        19,     109);

    // Draw standard X O
    Graphics_setForegroundColor(&g_sContext, FOREGROUNDCOLOR);
    Graphics_drawStringCentered(&g_sContext, symbolMap[map[0]], -1,  32,  32, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, symbolMap[map[1]], -1,  64,  32, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, symbolMap[map[2]], -1,  96,  32, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, symbolMap[map[3]], -1,  32,  64, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, symbolMap[map[4]], -1,  64,  64, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, symbolMap[map[5]], -1,  96,  64, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, symbolMap[map[6]], -1,  32,  96, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, symbolMap[map[7]], -1,  64,  96, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, symbolMap[map[8]], -1,  96,  96, OPAQUE_TEXT);
}

void DrawWinner(tcellstate map[9], int winner, uint32_t color) {
    int8_t *symbolMap[3] = {" ", "X", "O"};

    Graphics_setForegroundColor(&g_sContext, color);

    switch (winner) {
    case 1: // row 1
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[0]], -1,  32,  32, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[1]], -1,  64,  32, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[2]], -1,  96,  32, OPAQUE_TEXT);
        break;
    case 2: // row 2
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[3]], -1,  32,  64, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[4]], -1,  64,  64, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[5]], -1,  96,  64, OPAQUE_TEXT);
        break;
    case 3: // row 3
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[6]], -1,  32,  96, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[7]], -1,  64,  96, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[8]], -1,  96,  96, OPAQUE_TEXT);
        break;
    case 4: // col 1
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[0]], -1,  32,  32, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[3]], -1,  32,  64, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[6]], -1,  32,  96, OPAQUE_TEXT);
        break;
    case 5: // col 2
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[1]], -1,  64,  32, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[4]], -1,  64,  64, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[7]], -1,  64,  96, OPAQUE_TEXT);
        break;
    case 6: // col 3
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[2]], -1,  96,  32, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[5]], -1,  96,  64, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[8]], -1,  96,  96, OPAQUE_TEXT);
        break;
    case 7: // diagonal
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[0]], -1,  32,  32, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[4]], -1,  64,  64, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[8]], -1,  96,  96, OPAQUE_TEXT);
        break;
    case 8: // back diagonal
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[2]], -1,  96,  32, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[4]], -1,  64,  64, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, symbolMap[map[6]], -1,  32,  96, OPAQUE_TEXT);
        break;
    }

}

void DrawUnsigned(unsigned line, char prefix[4], unsigned n) {
    char buf[13]    = "PPPP XXXXXXXX";
    char hexstr[16] = "0123456789ABCDEF";
    buf[0]  = prefix[0];
    buf[1]  = prefix[1];
    buf[2]  = prefix[2];
    buf[3]  = prefix[3];
    buf[4]  = ' ';
    buf[5]  = hexstr[(n >> 28) & 0xF];
    buf[6]  = hexstr[(n >> 24) & 0xF];
    buf[7]  = hexstr[(n >> 20) & 0xF];
    buf[8]  = hexstr[(n >> 16) & 0xF];
    buf[9]  = hexstr[(n >> 12) & 0xF];
    buf[10] = hexstr[(n >>  8) & 0xF];
    buf[11] = hexstr[(n >>  4) & 0xF];
    buf[12] = hexstr[(      n) & 0xF];
    Graphics_drawStringCentered(&g_sContext, (int8_t *) buf,
                                             13, 64,
                                             16 + (line - 1) * 16,
                                             OPAQUE_TEXT);
}

void LCDDrawChar(unsigned row, unsigned col, int8_t c) {
    Graphics_drawString(&g_sContext,
                        &c,
                        1,
                        8 * (col % 16),
                        16 * (row % 8),
                        OPAQUE_TEXT);
}

void PrintString(char *str, int row, int col) {
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        LCDDrawChar(row,  col, str[i]);
        col++;
        if (col >= 16) {
            col = 0;
            row++;
            if (row >= 8) {
                row = 0;
            }
        }
    }
}



void LCDSetFgColor(color_t c) {
    switch (c)
       {
       case black:
           Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
           break;
       case red:
           Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
           break;
       case green:
           Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
           break;
       case yellow:
           Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_YELLOW);
           break;
       case blue:
           Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
           break;
       case magenta:
           Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_MAGENTA);
           break;
       case cyan:
           Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_CYAN);
           break;
       case white:
           Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
           break;
       }
}

void DrawIdle (int o3s, int Computer, int User)
{
    typedef enum {Mess1, Mess2, Mess3, Mess4} Message;
    static Message State = Mess1;
    if (o3s)
    {
    switch(State)
        {
    case Mess1:
        LCDSetFgColor(yellow);
        PrintString("* TicTacTone *", 0,1);
        State = Mess2;
        LCDSetFgColor(cyan);
        break;
    case Mess2:
        LCDSetFgColor(yellow);
        PrintString("              ", 0,1);
        PrintString("Score", 0,1);
        DrawScoreFinal(Computer,User);
        State = Mess3;
        LCDSetFgColor(cyan);
        break;
    case Mess3:
        LCDSetFgColor(yellow);
        PrintString("              ", 0,1);
        PrintString("S1: I start", 0,2);
        State = Mess4;
        LCDSetFgColor(cyan);
        break;
    case Mess4:
        LCDSetFgColor(yellow);
        PrintString("              ", 0,1);
        PrintString("S2: You start", 0,2);
        State = Mess1;
        LCDSetFgColor(cyan);
        break;
        }
    }
}

void DrawScoreFinal (int Computer, int User)
{
    if (Computer == 0)
        PrintString("00 - ", 0,7);
    else if (Computer == 1)
        PrintString("01 - ", 0,7);
    else if (Computer == 2)
        PrintString("02 - ", 0,7);
    else if (Computer == 3)
        PrintString("03 - ", 0,7);
    else if (Computer == 4)
        PrintString("04 - ", 0,7);
    else if (Computer == 5)
        PrintString("05 - ", 0,7);
    else if (Computer == 6)
        PrintString("06 - ", 0,7);
    else if (Computer == 7)
        PrintString("07 - ", 0,7);
    else if (Computer == 8)
        PrintString("08 - ", 0,7);

    if (User == 0)
        PrintString("00", 0,12);
    else if (User == 1)
        PrintString("01", 0,12);
    else if (User == 2)
        PrintString("02", 0,12);
    else if (User == 3)
        PrintString("03", 0,12);
    else if (User == 4)
        PrintString("04", 0,12);
    else if (User == 5)
        PrintString("05", 0,12);
    else if (User == 6)
        PrintString("06", 0,12);
    else if (User == 7)
        PrintString("07", 0,12);
    else if (User == 8)
        PrintString("08", 0,12);


}


