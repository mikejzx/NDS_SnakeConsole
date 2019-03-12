
/*
------------------------------------------
	Nintendo DS Lite Snake Console Game
		Developed By Michael
	Project begun: 10.03.2019, @ ~20:29
------------------------------------------
*/

// Includes
#include <nds.h>
#include <stdio.h>
#include "defines.h"
#include "structure.h"
#include "consoles.h"

struct Vector2 {
	int x;
	int y;

	bool Equals (int X, int Y) {
		return x == X && y == Y;
	}

	Vector2& Add (int X, int Y) {
		x += X;
		y += Y;
		return *this;
	}

	// = Operator
	Vector2& operator=(Vector2 a) { x = a.x; y = a.y; return *this; }
	// + Operator
	Vector2& operator+(Vector2 a) { x += a.x; y += a.y; return *this; }
	Vector2& operator+=(Vector2 a) { return *this + a; }
	// - Operator
	Vector2& operator-(Vector2 a) { x -= a.x; y -= a.y; return *this; }
	Vector2& operator-=(Vector2 a) { return *this - a; }
	// * Operator
	Vector2& operator*(int a) { x *= a; y *= a; return *this; }
	Vector2& operator*=(int a) { return *this * a; }
	// / Operator
	Vector2& operator/(int a) { x /= a; y /= a; return *this; }
	Vector2& operator/=(int a) { return *this / a; }
	// Conditional operators.
	bool operator==(const Vector2& a) const { return a.x == x && a.y == y; }
	bool operator!=(const Vector2& a) const { return a.x != x || a.y != y; }
};

struct TailNode {
	TailNode* both = NULL; // Xor of previous and next node
	Vector2 pos;

	TailNode(Vector2 data) {
		pos = data;
		both = NULL;
	}
};

// Linked list of tail. (11.03.2019: Finally implemented XOR-linked-list!!!)
class TailList {
	public:
		TailNode* head;
		TailNode* tail;
		int length = 0;

		static TailNode* Xor (TailNode* a, TailNode* b) {
			return (TailNode*)((uintptr_t)a ^ (uintptr_t)b);
		}

		void Reinitialise () {
			if (length > 0) {
				TailNode* cur = head;
				TailNode* prev = NULL;
				TailNode* next;
				while (1) {
					next = TailList::Xor(prev, cur->both);
					prev = cur;
					if (cur != NULL) {
						delete cur;
					}
					cur = next;
					
					if (cur == NULL) { break; }
				}
			}

			length = 0;
			head = NULL;
			tail = NULL;
		}

		void Append(Vector2 pos) {
			TailNode* temp = new TailNode(pos);
			temp->both = Xor(tail, NULL);

			if (length > 0) {
				tail->both = Xor(temp, Xor(tail->both, NULL));
			}
			else {
				head =  temp;
			}
			tail = temp;

			++length;
		}

		// ADDRESS OF NEXT NODE IS XOR(CURNODE, NULL)!!!
		void Prepend(Vector2 pos) {
			TailNode* temp = new TailNode(pos);
			temp->both = Xor(head, NULL);

			if (length > 0) {
				head->both = Xor(temp, Xor(head->both, NULL));
			}
			else {
				tail = temp;
			}
			head = temp;
			
			++length;
		}
};

class Player {
	public:
		Vector2 pos;
		Vector2 velo;
		TailList tail;

		void Reset() {
			pos = P_START_POS;
			velo = P_START_VELO;
			veloTime = VELO_DELAYER;

			tail.Reinitialise();
			// Starting at zero would result in a node 
			// at the head's position...
			for (int i = 1; i < 5; i++) {
				tail.Append({ pos.x - i, pos.y });
			}

			SetDelayer(VELO_DELAYER);
		}

		void ApplyInput (int input) {
			switch (input) {
				case (KEY_LEFT): { velo = { -1, 0 }; } break;
				case (KEY_RIGHT): { velo = { 1, 0 }; } break;
				case (KEY_UP): { velo = { 0, -1 }; } break;
				case (KEY_DOWN): { velo = { 0, 1 }; } break;
			}

			veloTimer++;
			if (veloTimer > veloTime) {
				Move();
				veloTimer = 0;
			}
		}

		void SetDelayer(int newDelay) {
			veloTime = max(newDelay, 1);
		}

	private:
		int veloTimer = 0;
		int veloTime = 25;

		// Wraps position between the borders of the game.
		static void PosWrap(Vector2* x) {
			// Wrap x
			if ((*x).x < 0) { (*x).x += BOARD_WIDTH; }
			if ((*x).x > BOARD_WIDTH - 1) { (*x).x -= BOARD_WIDTH; }

			// Wrap y
			if ((*x).y < 0) { (*x).y += BOARD_HEIGHT; }
			if ((*x).y > BOARD_HEIGHT - 1) { (*x).y -= BOARD_HEIGHT; }
		}

		void Move() {
			// Working reverse traversal!!!!!! (11.03.2019)
			//https://stackoverflow.com/questions/16138998/how-exactly-does-a-xor-linked-list-work
			// address(prev) = address(next) ^ currentNode(link)
			// address(next) = address(prev) ^ currentNode(link)
			TailNode* cur = tail.tail;
			TailNode* next = NULL;
			TailNode* prev;
			while (1) {
				prev = TailList::Xor(next, cur->both);
				next = cur;

				if (prev != NULL) {
					// Set position
					cur->pos = prev->pos;
					PosWrap(&cur->pos);
				}
				else {
					break;
				}

				cur = prev;
				//if (cur == NULL) { break; }
			}
			tail.head->pos = pos;
			PosWrap(&tail.head->pos);

			pos += velo;
			PosWrap(&pos);
		}
};

class Game {
	public:
		Player player;
		unsigned int tiles[BOARD_WIDTH][BOARD_HEIGHT];
		bool started = false;
		bool gameLost = false;

		void Start () {
			ResetGame();
			started = true;
		}

		void ResetGame() {
			player.Reset();
			levelTarget = 100;
			level = 0;
			levelProgress = 0;
		}

		void Update(int input) {
			GameManage();
			player.ApplyInput(input);
			RefreshTiles();
			DebugInfo();
			DrawBoard();
		}

		void StopGame () {
			started = false;
		}

	private:
		int levelTarget = 100;
		int level = 0;
		int levelProgress = 0;

		void DebugInfo () {
			#if DEBUGMODE == 1
			SelectConsole(CONSOLE_BTM);
			ClearConsole();
			iprintf("\n - Progress=%d\n - Level=%d\n - Target=%d", levelProgress, level, levelTarget);
			#endif
		}

		void GameManage() {
			// On game loss
			if (gameLost) {
				SelectConsole(CONSOLE_TOP);
				ClearConsole();
				RefreshTiles();
				DrawBoard();

				SelectConsole(CONSOLE_BTM);
				ClearConsole();
				iprintf("\n\x1b[10;11HYou Lose!");

				// Wait 25 frames
				for (int i = 0; i < 25; i++) {
					swiWaitForVBlank();
				}

				ClearConsole();

				ResetGame();
				gameLost = false;
			}

			// Handle level management
			levelProgress++;
			if (levelProgress > levelTarget) {
				levelProgress = 0;
				level++;
				levelTarget = 100 + (level * 40);

				// Affect game based on lvl
				player.SetDelayer(VELO_DELAYER - (level * 2));

				player.tail.Append(player.tail.tail->pos); //.Add(-player.velo.x, -player.velo.y)

			}
		}

		void RefreshTiles () {
			for (int y = 0; y < BOARD_HEIGHT; y++) {
				for (int x = 0; x < BOARD_WIDTH; x++) {
					if (player.pos.Equals(x, y)) {
						tiles[x][y] = TILE_HEAD;
					}
					else {
						tiles[x][y] = TILE_DEFAULT;
					}
				}
			}

			TailNode* cur = player.tail.head;
			TailNode* prev = NULL;
			TailNode* next;
			int i = 0;
			while (1) {
				// Start after 0.
				unsigned int t = TILE_TAIL;
				if (cur->pos == player.pos) {
					t = TILE_LOSEPOINT;
					gameLost = true;
				}
				tiles[cur->pos.x][cur->pos.y] = t;

				// Next is XOR of prev & cur
				next = TailList::Xor(prev, cur->both);
				prev = cur;
				cur = next;
				
				if (cur == NULL) {
					break;
				}
				i++;
			}
		}

		void DrawBoard() {
			SelectConsole(CONSOLE_TOP);
			void(*col)(int, int*) = [](int tileCol, int* lastConsoleCol) {
				if (*lastConsoleCol != tileCol) {
					SetConsoleColour(tileCol);
					*lastConsoleCol = tileCol;
				}
			};

			int lastConsoleCol = TILECOL_DEFAULT;
			for (int y = 0; y < BOARD_HEIGHT; y++) {
				for (int x = 0; x < BOARD_WIDTH; x++) {
					switch (tiles[x][y]) {
						case (TILE_DEFAULT): {
							col(TILECOL_DEFAULT, &lastConsoleCol);
							iprintf(TILECHAR_DEFAULT);
						} break;

						case (TILE_HEAD): {
							col(TILECOL_HEAD, &lastConsoleCol);
							iprintf(TILECHAR_HEAD);
						} break;

						case (TILE_TAIL): {
							col(TILECOL_TAIL, &lastConsoleCol);
							iprintf(TILECHAR_TAIL);
						} break;

						case (TILE_LOSEPOINT): {
							col(TILECOL_LOSEPOINT, &lastConsoleCol);
							iprintf(TILECHAR_LOSEPOINT);
						} break;

						default: {
							col(CONSCOL_I_PNK, &lastConsoleCol);
							iprintf("??");
						}
					}

					if (x >= BOARD_WIDTH - 1) {
						iprintf("\n");
					}
				}
			}
		}
};

// Main variables
bool inMenu = false;
bool gamePaused = false;
int input = 0;
Game game;

// (Consoles now in consoles.h)

// Prototype Functions:
void MenuView(); // When on menu screen
void GameView(); // When on game screen
void PauseView();
void HandleInput();
void PrintCentredTitle(const char*);

int main(void) {
	setBrightness(3, 0);
	videoSetMode(MODE_0_2D);
	videoSetModeSub(MODE_0_2D);
	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankC(VRAM_C_SUB_BG);

	InitialiseConsoles();
	consoleSelect(&topScreen);
	
	// Start in menu on launch.
	inMenu = true;
	game.ResetGame();

	while(1) {
		HandleInput();

		// Display menu if applicable
		if (inMenu) {
			MenuView();
		}
		else {
			if (gamePaused) {
				PauseView();
			}
			else {
				// Go straight to the game.
				ClearConsole();
				GameView();

				// Pause game on start click.
				if (input == KEY_START) {
					consoleSelect(&bottomScreen);
					consoleClear();
					gamePaused = true;
				}
			}
		}

		swiWaitForVBlank();
	}

	return 0;
}

void MenuView () {
	consoleSelect(&bottomScreen);
	consoleClear();

	PrintCentredTitle(" Main Menu ");

	// Print stuff In centre of screen
	iprintf("\n\x1b[10;5HPress START to Play !");

	// If player hits start, switch to game view, and
	// reset everything
	if (input == KEY_START) {
		consoleSelect(&bottomScreen);
		consoleClear();
		inMenu = false;
	}
}

void GameView () {
	if (!game.started) {
		game.Start();
	}

	// Print game on top screen
	game.Update(input);
}

void PauseView(){
	consoleSelect(&bottomScreen);
	consoleClear();

	PrintCentredTitle(" Pausesd ");
	iprintf("\n\x1b[10;5HPress START to Resume...");

	if (input == KEY_START) {
		consoleSelect(&bottomScreen);
		consoleClear();
		gamePaused = false;
	}
}

void HandleInput() {
	scanKeys();
	input = keysDown();
}

// Sweet custom title centreing algorithm :D
// Doesn't work well with all strings because
// division result's aren't in decimal format.
void PrintCentredTitle(const char* title) {
	//size_t len = sizeof(title);
	unsigned int len = strlen(title);
	unsigned int i; // Stored in local var incase i get carried away with for-loops
	for (i = 0; i < CHAR_H; i++) {
		if (i < ((CHAR_H / 2) - (len / 2)) ||
			i > ((CHAR_H / 2) + (len / 2))) {
			iprintf("=");
		}
		else {
			iprintf("%c", title[i - ((CHAR_H / 2) - (len / 2))]);
		}
	}
}