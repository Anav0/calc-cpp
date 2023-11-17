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

static int SCREEN_WIDTH = 1024;
static int SCREEN_HEIGHT = 720;

static bool SHOULD_QUIT = false;

SDL_Point   MOUSE_POS;

TTF_Font*   FONT;

struct Cell {
	SDL_Rect rect;
	SDL_bool isSelected = SDL_FALSE;
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
};

static std::vector<Cell> CELLS;
static std::vector<Column> COLUMNS;
static std::vector<Row> ROWS;

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

	if ( TTF_Init() < 0 ) {
		printf("Error initializing SDL_ttf: %s", TTF_GetError());
	}

	const char * font_path = "./assets/fonts/Lato-Regular.ttf";
	FONT = TTF_OpenFont(font_path , 14);

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

void update_rows() {
	int y = COL_H;
	bool exceededScreenHeight = false;
	ROWS.clear();

	while (!exceededScreenHeight) {
		Row row{};

		SDL_Rect rect{};
		rect.w = ROW_W;
		rect.h = ROW_H;
		rect.x = 0;
		rect.y = y;
		row.rect = rect;

		y += ROW_H;
		exceededScreenHeight = (rect.y + ROW_H) > SCREEN_HEIGHT;

		ROWS.push_back(row);
	}
}

void update_columns(SDL_Renderer* renderer) {
	int x = ROW_W;
	bool exceededScreenWidth = false;
	SDL_Color fontColor { 0, 0, 0 };
	SDL_Color bgColor { 255, 255, 255, 255 };

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
		SDL_Surface* text = TTF_RenderText_Shaded(FONT, s.c_str(), fontColor, bgColor);
		col.textRect = { col.rect.x + COL_W / 2, col.rect.y, text->w, text->h };
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
	update_rows();
	update_columns(renderer);
}

void handleMouseMotion(SDL_Event* e) {
	MOUSE_POS.x = e->motion.x;
	MOUSE_POS.y = e->motion.y;
}

void handleMouseButtonDown(SDL_Event* e) {
	switch (e->button.button) {
	case SDL_BUTTON_LEFT:
		for (Cell& cell : CELLS) {
			cell.isSelected = SDL_PointInRect(&MOUSE_POS, &cell.rect);
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

	case SDL_MOUSEMOTION:
		handleMouseMotion(&e);

	case SDL_MOUSEBUTTONDOWN:
		handleMouseButtonDown(&e);

	case SDL_WINDOWEVENT:
		handleWindowEvent(renderer, &e);
	}
}

int main(int argc, char* argv[])
{
	SDL_Renderer* renderer = 0;
	SDL_Window* window = 0;

	if (!init(&window, &renderer)) {
		return 0;
	}

	update(renderer);

	while (!SHOULD_QUIT)
	{
		handleEvents(renderer);

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
		}

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();

	return 0;
}
