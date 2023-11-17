#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdbool.h>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>

#include "Base.h"
#include "ProgramState.h"

static ProgramState STATE{};

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
	*window = SDL_CreateWindow("Sokoban", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, STATE.SCREEN_WIDTH, STATE.SCREEN_HEIGHT, windowFlags);
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
	STATE.FONT = TTF_OpenFont(font_path, 14);

	if (!STATE.FONT) {
		printf("Failed to load STATE.FONT: '%s' with msg: '%s'", font_path, TTF_GetError());
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

	for (const Cell& cell : STATE.CELLS)
	{
		SDL_SetRenderDrawColor(renderer, 192, 192, 192, 0xFF);

		if (SDL_PointInRect(&STATE.MOUSE_POS, &cell.rect)) {
			SDL_SetRenderDrawColor(renderer, 38, 87, 82, 0xFF);
		}

		if (STATE.SELECTED_CELL == &cell) {
			if (STATE.CURRENT_MODE == Edit)
				SDL_SetRenderDrawColor(renderer, 255, 188, 71, 0xFF);
			else
				SDL_SetRenderDrawColor(renderer, 38, 87, 82, 0xFF);
		}

		SDL_RenderDrawRect(renderer, &cell.rect);

		if (cell.contentTexture != NULL)
			SDL_RenderCopy(renderer, cell.contentTexture, NULL, &cell.contentRect);
	}

	for (const Column& column : STATE.COLUMNS)
	{
		SDL_SetRenderDrawColor(renderer, 248, 249, 250, 0xFF);
		SDL_RenderFillRect(renderer, &column.rect);
		SDL_SetRenderDrawColor(renderer, 67, 66, 61, 0xFF);
		SDL_RenderDrawRect(renderer, &column.rect);

		SDL_RenderCopy(renderer, column.textTexture, NULL, &column.textRect);
	}

	for (const Row& row : STATE.ROWS)
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
	int y = STATE.COL_H;
	bool exceededScreenHeight = false;

	STATE.ROWS.clear();
	int i = 1;

	while (!exceededScreenHeight) {
		Row row{};

		SDL_Rect rect{};
		rect.w = STATE.ROW_W;
		rect.h = STATE.ROW_H;
		rect.x = 0;
		rect.y = y;
		row.rect = rect;

		std::string s = std::to_string(i);
		SDL_Surface* text = TTF_RenderText_Blended(STATE.FONT, s.c_str(), STATE.FONT_COLOR);

		row.textRect = { rect.x + ((row.rect.w - text->w) / 2), rect.y + ((row.rect.h - text->h) / 2), text->w, text->h };
		row.textTexture = SDL_CreateTextureFromSurface(renderer, text);

		y += STATE.ROW_H;
		exceededScreenHeight = (rect.y + STATE.ROW_H) > STATE.SCREEN_HEIGHT;

		STATE.ROWS.push_back(row);
		i++;
	}
}

void update_columns(SDL_Renderer* renderer) {
	int x = STATE.ROW_W;
	bool exceededScreenWidth = false;

	STATE.COLUMNS.clear();
	int i = 0;
	while (!exceededScreenWidth) {
		Column col{};

		SDL_Rect rect{};
		rect.w = STATE.COL_W;
		rect.h = STATE.COL_H;
		rect.x = x;
		rect.y = 0;
		col.rect = rect;

		std::string s(1, char(65 + i));
		SDL_Surface* text = TTF_RenderText_Blended(STATE.FONT, s.c_str(), STATE.FONT_COLOR);

		col.textRect = { rect.x + ((col.rect.w - text->w) / 2), rect.y + ((col.rect.h - text->h) / 2), text->w, text->h };
		col.textTexture = SDL_CreateTextureFromSurface(renderer, text);

		x += STATE.COL_W;
		exceededScreenWidth = (rect.x + STATE.COL_W) > STATE.SCREEN_WIDTH;

		STATE.COLUMNS.push_back(col);
		i++;
	}

}

void update_cells() {
	int y = STATE.COL_H, x = STATE.ROW_W;
	bool screenIsNotFilled = true;
	STATE.CELLS.clear();

	while (screenIsNotFilled)
	{
		SDL_Rect cellRect{};
		cellRect.w = 100;
		cellRect.h = 20;
		cellRect.x = x;

		if (cellRect.x > STATE.SCREEN_WIDTH) {
			y += cellRect.h;
			x = STATE.ROW_W;
		}
		else {
			x += cellRect.w;
		}
		cellRect.y = y;

		Cell cell{};
		cell.rect = cellRect;

		STATE.CELLS.push_back(cell);

		screenIsNotFilled = y < STATE.SCREEN_HEIGHT;
	}
}

void update(SDL_Renderer* renderer) {
	update_cells();
	update_rows(renderer);
	update_columns(renderer);
}

void handleEvents(SDL_Renderer* renderer) {
	SDL_Event e;

	SDL_WaitEvent(&e);

	Mode* current_mode = STATE.getCurrentMode();
	current_mode->handleEvents(renderer, &e, &STATE);
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

	STATE.SELECTED_CELL = &STATE.CELLS[0];

	while (!STATE.SHOULD_QUIT)
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
