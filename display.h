#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "maplogic.h"
#include <ti/grlib/grlib.h>
typedef enum {black, red, green, yellow, blue, magenta, cyan, white} color_t;


/* palette */
#define BACKGROUNDCOLOR GRAPHICS_COLOR_PURPLE
#define LINECOLOR       GRAPHICS_COLOR_WHITE
#define FOREGROUNDCOLOR GRAPHICS_COLOR_CYAN
#define EMPHASISCOLOR   GRAPHICS_COLOR_YELLOW

void InitDisplay();
void DrawTime   (unsigned minutes, unsigned seconds);
void DrawMessage(char *s, uint32_t color);
void DrawScore  (int computerscore, int humanscore, uint32_t color);
void DrawBoard  (tcellstate map[9]);
void DrawWinner (tcellstate map[9],int winner, uint32_t color);
void DrawUnsigned(unsigned line, char prefix[4], unsigned n);
void PrintString(char *str, int row, int col);
void LCDSetFgColor(color_t c);
void LCDDrawChar(unsigned row, unsigned col, int8_t c);
void DrawIdle (int o3s,int Computer, int User);
#endif /* DISPLAY_H_ */
