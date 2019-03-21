
/*
------------------------------------------
	Nintendo DS Lite Snake Console Game
		Developed By Michael
	Project begun: 10.03.2019, @ ~20:29
------------------------------------------
*/

// Const
// Operator optimisations
// Pass by reference through methods

// Includes
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "defines.h"
#include "structure.h"
#include "consoles.h"
#include "xorlinkedlist.h"

struct Vector2 {
	int x;
	int y;

	bool Equals (int X, int Y) const {
		return x == X && y == Y;
	}

	Vector2& Add (int X, int Y) {
		x += X;
		y += Y;
		return *this;
	}

	// Operator overloads:
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

struct CoinData {
	Vector2 pos;

	CoinData() : pos({0, 0}) {}
	CoinData(Vector2& position) : pos(position) {}

	virtual void Update () {}
};

// Unused currently... (W.I.P)
struct CoinDataDynamic : public CoinData {
	Vector2 velo;

	CoinDataDynamic(Vector2& pos) : CoinData(pos) {
		// Set velocity to random.
		int iRandX = (rand() % 2) - 1,
			iRandY = (rand() % 2) - 1;
		velo = { iRandX, iRandY };
	}

	void Update () override {
		CoinData::Update(); // Call base virtual method

		// Apply velocity to position.
		pos += velo;
	}
};

// This coin will make the player lose 2 tail parts. If their length is 2, then they lose
struct CoinDataTrap : public CoinData {
	CoinDataTrap(Vector2& pos) : CoinData(pos) {

	}

	void Update () override {

	}
};

// Linked list of tail. (11.03.2019: Finally implemented XOR-linked-list!!!)
//class TailList {} (Now obsolete. Replaced with NodeCollection)

class Player {
	public:
		Vector2 pos, velo;
		NodeCollection<Vector2> tail;

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

		void Add1ToTail () {
			tail.Append(tail.tail->data); //.Add(-velo.x, -velo.y)
		}

	private:
		int veloTimer = 0, veloTime = 25;

		// Wraps position between the borders of the game.
		static void PosWrap(Vector2& x) {
			// Wrap x
			if (x.x < 0) { x.x += BOARD_WIDTH; }
			if (x.x > BOARD_WIDTH - 1) { x.x -= BOARD_WIDTH; }

			// Wrap y
			if (x.y < 0) { x.y += BOARD_HEIGHT; }
			if (x.y > BOARD_HEIGHT - 1) { x.y -= BOARD_HEIGHT; }
		}

		void Move() {
			// Working reverse traversal!!!!!! (11.03.2019)
			//https://stackoverflow.com/questions/16138998/how-exactly-does-a-xor-linked-list-work
			// address(prev) = address(next) ^ currentNode(link)
			// address(next) = address(prev) ^ currentNode(link)
			Node<Vector2>* cur = tail.tail, *next = NULL, *prev;
			while (1) {
				prev = NodeCollection<Vector2>::Xor(next, cur->both);
				next = cur;

				if (prev != NULL) {
					// Set position
					cur->data = prev->data;
					PosWrap(cur->data);
				}
				else {
					break;
				}

				cur = prev;
			}
			tail.head->data = pos;
			PosWrap(tail.head->data);

			pos += velo;
			PosWrap(pos);
		}
};

static Player player;

class ScoringManager {
	public:
		NodeCollection<CoinData> coins;
		int coinTimer = 0;
		int coinTime = 100;
		int coinMaximum = 3;

		int hiscore = 0;
		int curScore;

		void Initialise () {
			srand(time(NULL));
		}

		void Update () {
			UpdateCoinTimer();
		}

		void Reset () {
			// Reset coins
			coins.Reinitialise();

			// Reset score
			curScore = 0;
		}

		void ScoreAdd(int increment) {
			curScore += increment;
		}

		void ScoreReset () {
			curScore = 0;
		}

		int GetCoinInstances() {
			return coins.length;
		}

		void CoinCollected () {
			ScoreAdd(1);
			player.Add1ToTail();
		}

	private:
		void UpdateCoinTimer () {
			if (GetCoinInstances() > coinMaximum - 1) { 
				coinTimer = 0;
				return; 
			}

			coinTimer++;
			if (coinTimer > coinTime) {
				coinTimer = 0;
				CoinSpawn();
			}
		}

		void CoinSpawn () {
			Vector2 pos = {
				rand() % (BOARD_WIDTH - 1) + 1, 
				rand() % (BOARD_HEIGHT - 1) + 1 
			};

			coins.Append(CoinData(pos));
		}
};

// Are static so they can be used in lambda expressions
static unsigned int tiles[BOARD_WIDTH][BOARD_HEIGHT];
static ScoringManager scoring;
static int* coinsDirty;

class SaveManager {
	// TODO: USE FOPEN, AND OTHER BUILT-IN FUNCTIONS TO WORK THIS...

	public:
		static void Save () {
			FILE* pFile = fopen(GAME_SAVE_PATH, "wb");
			
			if (pFile != NULL) {
				fputs("save data working", pFile);
				fclose(pFile);
			}
		}
};

class Game {
	public:
		bool started = false, gameLost = false;

		void Start () {
			ResetGame();
			started = true;
		}

		void ResetGame() {
			player.Reset();
			levelTarget = 100;
			level = 0;
			levelProgress = 0;
			scoring.ScoreReset();
		}

		void Update(int& input) {
			GameManage();
			player.ApplyInput(input);
			scoring.Update();
			RefreshTiles();
			DebugInfo();
			DrawBoard();
		}

		void StopGame () {
			started = false;
		}

	private:
		int levelTarget = 100, level = 0, levelProgress = 0;
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
				//player.Add1ToTail();
			}
		}

		void DrawHeader () {
			SelectConsole(CONSOLE_TOP);
			printf("Score: %d\n", scoring.curScore);
		}

		static void SetCoinTile(CoinData d, int idx) {
			if (d.pos == player.pos) {
				coinsDirty[idx] = 1;
				// Collected coin. Remove from list.
				scoring.CoinCollected();
				//scoring.coins.RemoveAt(scoring.coins.IndexOf(&d));
			}
			else {
				// Only draw coin if player is not on it.
				tiles[d.pos.x][d.pos.y] = TILE_COIN;
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

			Node<Vector2>* cur = player.tail.head, *prev = NULL, *next;
			int i = 0;
			while (1) {
				// Start after 0.
				unsigned int t = TILE_TAIL;
				if (cur->data == player.pos) {
					t = TILE_LOSEPOINT;
					gameLost = true;
				}
				tiles[cur->data.x][cur->data.y] = t;

				// Next is XOR of prev & cur
				next = NodeCollection<Vector2>::Xor(prev, cur->both);
				prev = cur; cur = next;
				if (cur == NULL) { break; } i++;
			}

			//void(*func)(CoinData) = [](CoinData x) { tiles[x.pos.x][x.pos.y] = TILE_COIN; };

			// Coins marked dirty are being removed.
			const int len = scoring.coins.length;
			coinsDirty = new int[len];
			for (int i = 0; i < len; i++) { coinsDirty[i] = 0; } // Initialise all as non-dirty
			scoring.coins.TraverseForward(SetCoinTile);
			for (int i = 0; i < len; i++) { 
				if (coinsDirty[i] == 1) {
					scoring.coins.RemoveAt(i);
				}
			}
			delete[] coinsDirty;
		}

		void DrawBoard() {
			SelectConsole(CONSOLE_TOP);
			void(*col)(int, int*) = [](int tileCol, int* lastConsoleCol) {
				if (*lastConsoleCol != tileCol) {
					SetConsoleColour(tileCol);
					*lastConsoleCol = tileCol;
				}
			};

			int lastConsoleCol = CONSCOL_I_WHT; // Must initialise as 1 because of end of this method.
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

						case (TILE_COIN): {
							col(TILECOL_COIN, &lastConsoleCol);
							iprintf(TILECHAR_COIN);
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
			iprintf(CONSCOL_WHT); // Reset at end
			DrawHeader();
		}
};

// Main variables
bool inMenu = false;
bool gamePaused = false;
int input = 0;
Game game;

// Sorta irrelevant, just additional cool thingies.
#define LOADING_CHAR_COUNT 5
#define LOADING_CHAR_DELAY 11
int loadingChar = 0;
int loadingCharTimer = 0;
char loadingChars[LOADING_CHAR_COUNT] = "|/-\\";

// (Consoles now in consoles.h)

// Prototype Functions:
void MenuView(); // When on menu screen
void GameView(); // When on game screen
void PauseView();
void HandleInput();
void PrintCentredTitle(const char*);
void OnPause ();

int main(void) {
	setBrightness(3, 0);
	videoSetMode(MODE_0_2D);
	videoSetModeSub(MODE_0_2D);
	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankC(VRAM_C_SUB_BG);

	InitialiseConsoles();
	consoleSelect(&topScreen);

	SaveManager::Save ();

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
					OnPause();
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

void OnPause () {
	loadingChar = 0;
}

void PauseView(){
	consoleSelect(&bottomScreen);
	consoleClear();

	PrintCentredTitle(" Pausesd ");
	iprintf("\n\x1b[10;5HPress START to Resume...");

	// Paying homage to the MS-DOS lol. (Really just though this looked cool)
	iprintf("\n\x1b[20;2H%c", loadingChars[loadingChar]);
	loadingCharTimer++;
	if (loadingCharTimer > LOADING_CHAR_DELAY) {
		loadingCharTimer = 0;
		loadingChar++;
		if (loadingChar > LOADING_CHAR_COUNT - 2) {
			loadingChar = 0;
		}
	}

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
	for (unsigned int i = 0; i < CHAR_H; i++) {
		if (i < ((CHAR_H / 2) - (len / 2)) ||
			i > ((CHAR_H / 2) + (len / 2))) {
			iprintf("=");
		}
		else {
			iprintf("%c", title[i - ((CHAR_H / 2) - (len / 2))]);
		}
	}
}