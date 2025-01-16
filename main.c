#include <stdlib.h>
#include <time.h>

#include "include/raylib.h"

#define SCR_WIDTH 1501
#define SCR_HEIGHT 1501

#define COLS 15
#define ROWS 15

#define MYBLACK CLITERAL(Color){ 20, 20, 20, 255 }

const int cell_width = SCR_WIDTH / COLS;
const int cell_height = SCR_HEIGHT / ROWS;

typedef struct Cell {
	int i;
	int j;
	bool contains_mine;
	bool revealed;
	bool flagged;
	int number;
} Cell;

Cell grid[COLS][ROWS];

Texture2D flag;
Texture2D mine;
int tiles_revealed;
int mines_present;

typedef enum GameState {
	PLAYING,
	LOSE,
	WIN
} GameState;

GameState state;

void draw_cell(Cell cell);
bool index_is_valid(int i, int j);
void grid_clear_near(int I, int J);
void reveal_cell(int i, int j);
int count_near_mines(int I, int J);
void flag_cell(int i, int j);
void grid_init(void);

int main(void)
{

	srand(time(0));

	// init game
	InitWindow(SCR_WIDTH, SCR_HEIGHT, "Minesweeper");
	// load icon
	Image icon = LoadImage("assets/icon.png");
	ImageFormat(&icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
	SetWindowIcon(icon);
	UnloadImage(icon);

	flag = LoadTexture("assets/flag.png");
	mine = LoadTexture("assets/mine.png");

	grid_init();
	tiles_revealed = 0;
	state = PLAYING;

	// run game
	while (!WindowShouldClose()) {

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			Vector2 mouse_pos = GetMousePosition();
			int index_i = mouse_pos.x / cell_width;
			int index_j = mouse_pos.y / cell_height;

			if (state == PLAYING && index_is_valid(index_i, index_j))
				reveal_cell(index_i, index_j);
		}
		else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
			Vector2 mouse_pos = GetMousePosition();
			int index_i = mouse_pos.x / cell_width;
			int index_j = mouse_pos.y / cell_height;

			if (index_is_valid(index_i, index_j))
				flag_cell(index_i, index_j);
		}

		if (IsKeyPressed(KEY_R)) {
			grid_init();
			tiles_revealed = 0;
			state = PLAYING;
		}

		if (IsKeyPressed(KEY_Q))
			break;

		BeginDrawing();
		{
			ClearBackground(MYBLACK);

			for (int i = 0; i < COLS; i++)
				for (int j = 0; j < ROWS; j++)
					draw_cell(grid[i][j]);

			if (state == LOSE) {
				DrawRectangle(0, 0, SCR_WIDTH, SCR_HEIGHT, Fade(WHITE, 0.7f));
				DrawText("YOU LOSE!", SCR_WIDTH / 2 - MeasureText("YOU LOSE!", 50) / 2, SCR_HEIGHT / 2 - 25, 50, RED);
				DrawText("Press 'R' to Play Again!", SCR_WIDTH / 2 - MeasureText("Press 'R' to Play Again!", 40) / 2, SCR_HEIGHT * 0.75f - 20, 40, DARKGRAY);
			}

			if (state == WIN) {
				DrawRectangle(0, 0, SCR_WIDTH, SCR_HEIGHT, Fade(WHITE, 0.7f));
				DrawText("YOU WIN!", SCR_WIDTH / 2 - MeasureText("YOU WIN!", 50) / 2, SCR_HEIGHT / 2 - 25, 50, GREEN);
				DrawText("Press 'R' to Play Again!", SCR_WIDTH / 2 - MeasureText("Press 'R' to Play Again!", 40) / 2, SCR_HEIGHT * 0.75f - 20, 40, DARKGRAY);
			}
		}
		EndDrawing();
	}

	// close game
	UnloadTexture(flag);
	UnloadTexture(mine);
	CloseWindow();

	return 0;
}

void draw_cell(Cell cell)
{
	if (cell.revealed) {
		if (cell.contains_mine) {
			DrawRectangle(cell.i * cell_width + 1, cell.j * cell_height + 1, cell_width, cell_height, RED);

			Rectangle source = {0, 0, mine.width, mine.height};
			Rectangle dest = {cell.i * cell_width, cell.j * cell_height, cell_width, cell_height};
			Vector2 origin = {0, 0};

			DrawTexturePro(mine, source, dest, origin, 0.0f, WHITE);
		}
		else {
			DrawRectangle(cell.i * cell_width + 1, cell.j * cell_height + 1, cell_width, cell_height, DARKGRAY);
			if (cell.number > 0)
				DrawText(TextFormat("%d", cell.number), cell.i * cell_width + 30, cell.j * cell_height + 6, cell_height - 20, BLACK);
		}
	}
	else if (cell.flagged) {
		Rectangle source = {0, 0, flag.width, flag.height};
		Rectangle dest = {cell.i * cell_width, cell.j * cell_height, cell_width, cell_height};
		Vector2 origin = {0, 0};

		DrawTexturePro(flag, source, dest, origin, 0.0f, WHITE);
	}

	DrawRectangleLines((cell.i * cell_width)+1, (cell.j * cell_height)+1, cell_width, cell_height, WHITE);
}

bool index_is_valid(int i, int j)
{
	return i >= 0 && i < COLS && j >= 0 && j < ROWS;
}

void grid_clear_near(int I, int J)
{
	for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++) {
			if (i == 0 && j == 0)
				continue;

			if (!index_is_valid(I + i, J + j))
				continue;

			if (!grid[I+i][J+j].revealed)
				reveal_cell(i + I, j + J);
		}
}

void reveal_cell(int i, int j)
{

	if (grid[i][j].flagged)
		return;

	grid[i][j].revealed = true;

	if (grid[i][j].contains_mine) {
		state = LOSE;
	}
	else {
		if (grid[i][j].number == 0) {
			grid_clear_near(i, j);
		}

		tiles_revealed++;

		if (tiles_revealed == ROWS * COLS - mines_present) {
			state = WIN;
		}
	}
}

int count_near_mines(int I, int J)
{
	int count = 0;
	for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++) {
			if (i == 0 && j == 0)
				continue;

			if (!index_is_valid(I + i, J + j))
				continue;

			if (grid[I+i][J+j].contains_mine)
				count++;
		}
	return count;
}

void flag_cell(int i, int j)
{
	if (grid[i][j].revealed)
		return;

	grid[i][j].flagged = !grid[i][j].flagged;
}

void grid_init(void)
{
	for (int i = 0; i < COLS; i++)
		for (int j = 0; j < ROWS; j++)
			grid[i][j] = (Cell) {
				.i = i,
				.j = j,
				.contains_mine = false,
				.revealed = false,
				.flagged = false,
				.number = -1
			};

	mines_present = (int) (ROWS * COLS * 0.1f);
	int mines_count = mines_present;
	while (mines_count > 0) {
		int i = rand() % COLS;
		int j = rand() % ROWS;

		if (!grid[i][j].contains_mine) {
			grid[i][j].contains_mine = true;
			mines_count--;
		}
	}

	for (int i = 0; i < COLS; i++)
		for (int j = 0; j < ROWS; j++)
			if (!grid[i][j].contains_mine)
				grid[i][j].number = count_near_mines(i, j);
}
