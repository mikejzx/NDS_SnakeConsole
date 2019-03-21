
// General defines
#define DEBUGMODE 0

const char* GAME_SAVE_PATH = "project_SNAKE2.0.SAV";

#define BOARD_WIDTH 16 // Width of game board
#define BOARD_HEIGHT 10 // Height of game board
#define CHAR_H 32 // Number of chars that fit horizontally
#define CHAR_V 10 // Number of chars that fit vertically
#define CONSOLE_TOP 0
#define CONSOLE_BTM 1
#define VELO_DELAYER 25

#define P_TAIL_START 4 // Length of tail at beginning.
#define P_START_POS { 5, 5 }
#define P_START_VELO { 1, 0 }

// Tilestate defines
#define TILE_DEFAULT 0 // Nothing on tile
#define TILE_HEAD 1 // Player head
#define TILE_TAIL 2 // Player Tail
#define TILE_LOSEPOINT 3 // Where player collides with Their tail.
#define TILE_ENEMY 4 // Unused currently
#define TILE_COIN 5 // Unused currently

// Tile colour defines (at index)
#define TILECOL_DEFAULT 0
#define TILECOL_HEAD 2
#define TILECOL_TAIL 2
#define TILECOL_LOSEPOINT 1
#define TILECOL_ENEMY 1
#define TILECOL_COIN 3

// Tile characters
#define TILECHAR_DEFAULT ". "
#define TILECHAR_HEAD "X "
#define TILECHAR_TAIL "Y "
#define TILECHAR_LOSEPOINT "# "
#define TILECHAR_ENEMY "* "
#define TILECHAR_COIN "C "

// ...

// Functional defines
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

// Colour foramtting escape codes for console.
const char* colourFormats[] = {
	(const char*)"\x1b[30;1m", // 0 Black
	(const char*)"\x1b[31;1m", // 1 Red
	(const char*)"\x1b[32;1m", // 2 Green
	(const char*)"\x1b[33;1m", // 3 Yellow
	(const char*)"\x1b[34;1m", // 4 Blue
	(const char*)"\x1b[35;1m", // 5 Pink
	(const char*)"\x1b[36;1m", // 6 Cyan
	(const char*)"\x1b[37;1m", // 7 White
};

// Console colour defines
#define CONSCOL_BLK "\x1b[30;1m" // Black
#define CONSCOL_RED "\x1b[31;1m" // Red
#define CONSCOL_GRN "\x1b[32;1m" // Green
#define CONSCOL_YLW "\x1b[33;1m" // Yellow
#define CONSCOL_BLU "\x1b[34;1m" // Blue
#define CONSCOL_PNK "\x1b[35;1m" // Pink
#define CONSCOL_CYN "\x1b[36;1m" // Cyan
#define CONSCOL_WHT "\x1b[37;1m" // White

// Console colours in integer form
#define CONSCOL_I_BLK 0 // Black
#define CONSCOL_I_RED 1 // Red
#define CONSCOL_I_GRN 2 // Green
#define CONSCOL_I_YLW 3 // Yellow
#define CONSCOL_I_BLU 4 // Blue
#define CONSCOL_I_PNK 5 // Pink
#define CONSCOL_I_CYN 6 // Cyan
#define CONSCOL_I_WHT 7 // White