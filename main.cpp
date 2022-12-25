#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdbool.h>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>

const int ROW_W = 40;
const int ROW_H = 20;

const int COL_W = 100;
const int COL_H = 20;

static int SCREEN_WIDTH = 1336;
static int SCREEN_HEIGHT = 768;

static bool SHOULD_QUIT = false;

#define MAX_TEXT_LEN 256

SDL_Point   MOUSE_POS;

TTF_Font* FONT;

struct Cell {
	SDL_Rect rect;
	SDL_bool isSelected = SDL_FALSE;

	SDL_Rect contentRect;
	SDL_Texture* contentTexture;
	char content[MAX_TEXT_LEN];
};

struct Column {
	SDL_Rect rect;
	SDL_bool isSelected = SDL_FALSE;

	SDL_Rect textRect;
	SDL_Texture* textTexture;
};

struct Row {
	SDL_Rect rect;
	SDL_bool isSelected = SDL_FALSE;

	SDL_Rect textRect;
	SDL_Texture* textTexture;
};

enum Mode {
	View,
	Edit,
};

static std::vector<Cell> CELLS;
static std::vector<Column> COLUMNS;
static std::vector<Row> ROWS;
static SDL_Color FONT_COLOR = { 0, 0, 0, 0 };

//char usersInput[MAX_TEXT_LEN];
Sint32 cursor;
Sint32 inputLength;

Cell* selectedCell;

void updateCellContentTexture(SDL_Renderer* renderer, Cell* cell) {

	SDL_Surface* text = TTF_RenderText_Blended(FONT, cell->content, FONT_COLOR);

	if (text == NULL) {
		selectedCell->contentTexture = NULL;
		return;
	}

	selectedCell->contentRect = { selectedCell->rect.x + ((selectedCell->rect.w - text->w) / 2), selectedCell->rect.y + ((selectedCell->rect.h - text->h) / 2), text->w, text->h };
	selectedCell->contentTexture = SDL_CreateTextureFromSurface(renderer, text);
}

bool init(SDL_Window** window, SDL_Renderer** renderer)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL2 could not be initialized!\n" "SDL2 Error: %s\n", SDL_GetError());
		return false;
	}

	int flags = IMG_INIT_PNG;
	if ((IMG_Init(flags) & flags) != flags) {
		printf("SDL2_image could not be initialized with PNG support!\n"
			"SDL2_image Error: %s\n", IMG_GetError());
		return false;
	}

	Uint32 windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
	*window = SDL_CreateWindow("Sokoban", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);
	if (!window)
	{
		printf("Window could not be created!\n" "SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	*renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
	{
		printf("Renderer could not be created!\n"
			"SDL_Error: %s\n", SDL_GetError());

		return false;
	}

	if (TTF_Init() < 0) {
		printf("Error initializing SDL_ttf: %s", TTF_GetError());
	}

	const char* font_path = "./assets/fonts/Lato-Regular.ttf";
	FONT = TTF_OpenFont(font_path, 14);

	if (!FONT) {
		printf("Failed to load font: '%s' with msg: '%s'", font_path, TTF_GetError());
	}

	return true;
}

SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* path, int* w, int* h)
{
	SDL_Texture* imageTexture = IMG_LoadTexture(renderer, path);
	if (!imageTexture)
	{
		printf("Unable to load image '%s'!\n" "SDL_image Error: %s", path, IMG_GetError());
		return NULL;
	}

	SDL_QueryTexture(imageTexture, NULL, NULL, w, h);
	return imageTexture;
}

void render(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);

	for (const Cell& cell : CELLS)
	{
		SDL_SetRenderDrawColor(renderer, 192, 192, 192, 0xFF);

		if (SDL_PointInRect(&MOUSE_POS, &cell.rect)) {
			SDL_SetRenderDrawColor(renderer, 38, 87, 82, 0xFF);
		}

		if (cell.isSelected) {
			SDL_SetRenderDrawColor(renderer, 203, 38, 6, 0xFF);
		}

		SDL_RenderDrawRect(renderer, &cell.rect);

		if (cell.contentTexture != NULL)
			SDL_RenderCopy(renderer, cell.contentTexture, NULL, &cell.contentRect);
	}

	for (const Column& column : COLUMNS)
	{
		SDL_SetRenderDrawColor(renderer, 248, 249, 250, 0xFF);
		SDL_RenderFillRect(renderer, &column.rect);
		SDL_SetRenderDrawColor(renderer, 67, 66, 61, 0xFF);
		SDL_RenderDrawRect(renderer, &column.rect);

		SDL_RenderCopy(renderer, column.textTexture, NULL, &column.textRect);
	}

	for (const Row& row : ROWS)
	{
		SDL_SetRenderDrawColor(renderer, 248, 249, 250, 0xFF);
		SDL_RenderFillRect(renderer, &row.rect);
		SDL_SetRenderDrawColor(renderer, 67, 66, 61, 0xFF);
		SDL_RenderDrawRect(renderer, &row.rect);

		SDL_RenderCopy(renderer, row.textTexture, NULL, &row.textRect);
	}

	SDL_RenderPresent(renderer);
}

void update_rows(SDL_Renderer* renderer) {
	int y = COL_H;
	bool exceededScreenHeight = false;

	ROWS.clear();
	int i = 1;

	while (!exceededScreenHeight) {
		Row row{};

		SDL_Rect rect{};
		rect.w = ROW_W;
		rect.h = ROW_H;
		rect.x = 0;
		rect.y = y;
		row.rect = rect;

		std::string s = std::to_string(i);
		SDL_Surface* text = TTF_RenderText_Blended(FONT, s.c_str(), FONT_COLOR);

		row.textRect = { rect.x + ((row.rect.w - text->w) / 2), rect.y + ((row.rect.h - text->h) / 2), text->w, text->h };
		row.textTexture = SDL_CreateTextureFromSurface(renderer, text);

		y += ROW_H;
		exceededScreenHeight = (rect.y + ROW_H) > SCREEN_HEIGHT;

		ROWS.push_back(row);
		i++;
	}
}

void update_columns(SDL_Renderer* renderer) {
	int x = ROW_W;
	bool exceededScreenWidth = false;

	COLUMNS.clear();
	int i = 0;
	while (!exceededScreenWidth) {
		Column col{};

		SDL_Rect rect{};
		rect.w = COL_W;
		rect.h = COL_H;
		rect.x = x;
		rect.y = 0;
		col.rect = rect;

		std::string s(1, char(65 + i));
		SDL_Surface* text = TTF_RenderText_Blended(FONT, s.c_str(), FONT_COLOR);

		col.textRect = { rect.x + ((col.rect.w - text->w) / 2), rect.y + ((col.rect.h - text->h) / 2), text->w, text->h };
		col.textTexture = SDL_CreateTextureFromSurface(renderer, text);

		x += COL_W;
		exceededScreenWidth = (rect.x + COL_W) > SCREEN_WIDTH;

		COLUMNS.push_back(col);
		i++;
	}

}

void update_cells() {
	int y = COL_H, x = ROW_W;
	bool screenIsNotFilled = true;
	CELLS.clear();

	while (screenIsNotFilled)
	{
		SDL_Rect cellRect{};
		cellRect.w = 100;
		cellRect.h = 20;
		cellRect.x = x;

		if (cellRect.x > SCREEN_WIDTH) {
			y += cellRect.h;
			x = ROW_W;
		}
		else {
			x += cellRect.w;
		}
		cellRect.y = y;

		Cell cell{};
		cell.rect = cellRect;

		CELLS.push_back(cell);

		screenIsNotFilled = y < SCREEN_HEIGHT;
	}
}

void update(SDL_Renderer* renderer) {
	update_cells();
	update_rows(renderer);
	update_columns(renderer);
}

void handleKeydown(SDL_Renderer* renderer, SDL_Event* e) {
	switch (e->key.keysym.sym) {
	case SDLK_BACKSPACE:
		if (selectedCell == NULL || selectedCell->content == "") return;

		selectedCell->content[strlen(selectedCell->content) - 1] = '\0';
		updateCellContentTexture(renderer, selectedCell);
	}
}

void handleMouseMotion(SDL_Event* e) {
	MOUSE_POS.x = e->motion.x;
	MOUSE_POS.y = e->motion.y;
}

void handleMouseButtonDown(SDL_Renderer* renderer, SDL_Event* e) {
	switch (e->button.button) {
	case SDL_BUTTON_LEFT:
		printf("Cell changed!\n");

		SDL_StopTextInput();

		for (Cell& cell : CELLS) {
			cell.isSelected = SDL_PointInRect(&MOUSE_POS, &cell.rect);

			if (cell.isSelected) {
				selectedCell = &cell;
				SDL_StartTextInput();
				SDL_SetTextInputRect(&cell.rect);
			}
		}
		break;

	case SDL_BUTTON_RIGHT:
		break;

	case SDL_BUTTON_MIDDLE:
		break;
	}

}

void handleWindowEvent(SDL_Renderer* renderer, SDL_Event* e) {
	switch (e->window.event) {
	case SDL_WINDOWEVENT_RESIZED:
		SDL_Log("Window %d resized to %dx%d", e->window.windowID, e->window.data1, e->window.data2);
		SCREEN_WIDTH = e->window.data1;
		SCREEN_HEIGHT = e->window.data2;
		update(renderer);
	}
}

void handleEvents(SDL_Renderer* renderer) {
	SDL_Event e;

	SDL_WaitEvent(&e);

	switch (e.type) {
	case SDL_QUIT:
		SHOULD_QUIT = true;
		break;

	case SDL_KEYDOWN:
		handleKeydown(renderer, &e);
		break;

	case SDL_TEXTINPUT:
		if (selectedCell != NULL) {
			if (SDL_strlen(selectedCell->content) + SDL_strlen(e.text.text) < MAX_TEXT_LEN) {
				SDL_strlcat(selectedCell->content, e.text.text, sizeof(selectedCell->content));

				updateCellContentTexture(renderer, selectedCell);
			}
			else {
				printf("Input is too big!\n");
			}
		}
		break;

	case SDL_MOUSEMOTION:
		handleMouseMotion(&e);
		break;

	case SDL_MOUSEBUTTONDOWN:
		handleMouseButtonDown(renderer, &e);
		break;

	case SDL_WINDOWEVENT:
		handleWindowEvent(renderer, &e);
		break;
	}
}


int main(int argc, char* argv[])
{
	SDL_Renderer* renderer = 0;
	SDL_Window* window = 0;

	if (!init(&window, &renderer)) {
		return 0;
	}

	SDL_StartTextInput();

	update(renderer);

	while (!SHOULD_QUIT)
	{
		handleEvents(renderer);

		render(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();

	return 0;
}
