
#include <nds.h>
#include <stdio.h>

PrintConsole topScreen;
PrintConsole bottomScreen;

void InitialiseConsoles () {
	consoleInit(&topScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
	consoleInit(&bottomScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);
}

void SetConsoleColour (int c) {
	if (c > 7) { c = 7; }
	if (c < 0) { c = 0; }
	iprintf("%s", colourFormats[c]);
}

void SetConsoleColour (const char* c) {
	iprintf("%s", c);
}

void ResetConsoleColour () { 
	iprintf("%s", CONSCOL_WHT); 
}

void SelectConsole (int id) {
	PrintConsole* c = &topScreen;
	if (id == CONSOLE_BTM) {
		c = &bottomScreen;
	}
	consoleSelect(c);
}

void ClearConsole () {
	consoleClear();
}