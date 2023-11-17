#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdbool.h>
#include <vector>
#include <cassert>

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
	*window = SDL_CreateWindow("Sokoban", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, STATE.screenWidth, STATE.screenHeight, windowFlags);
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

	const char* font_path = "./assets/fonts/Poppins-Regular.ttf";
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

	for (const Cell& cell : STATE.cells)
	{
		SDL_SetRenderDrawColor(renderer, 192, 192, 192, 0xFF);

		if (SDL_PointInRect(&STATE.mousePos, &cell.rect))
			SDL_SetRenderDrawColor(renderer, STATE.hoverColor.r, STATE.hoverColor.g, STATE.hoverColor.b, STATE.hoverColor.a);
		
		if(STATE.subjectCell == &cell)
			SDL_SetRenderDrawColor(renderer, STATE.subjectColor.r, STATE.subjectColor.g, STATE.subjectColor.b, STATE.subjectColor.a);

		if (STATE.selectedCell == &cell)
			SDL_SetRenderDrawColor(renderer, STATE.selectedColor.r, STATE.selectedColor.g, STATE.selectedColor.b, STATE.selectedColor.a);

		SDL_RenderDrawRect(renderer, &cell.rect);

		if (cell.contentTexture != NULL)
			SDL_RenderCopy(renderer, cell.contentTexture, NULL, &cell.contentRect);
	}

	for (const Column& column : STATE.columns)
	{
		SDL_SetRenderDrawColor(renderer, 248, 249, 250, 0xFF);
		SDL_RenderFillRect(renderer, &column.rect);
		SDL_SetRenderDrawColor(renderer, 67, 66, 61, 0xFF);
		SDL_RenderDrawRect(renderer, &column.rect);

		SDL_RenderCopy(renderer, column.textTexture, NULL, &column.textRect);
	}

	for (const Row& row : STATE.rows)
	{
		SDL_SetRenderDrawColor(renderer, 248, 249, 250, 0xFF);
		SDL_RenderFillRect(renderer, &row.rect);
		SDL_SetRenderDrawColor(renderer, 67, 66, 61, 0xFF);
		SDL_RenderDrawRect(renderer, &row.rect);

		SDL_RenderCopy(renderer, row.textTexture, NULL, &row.textRect);
	}

	if (STATE.currentMode == Edit) {
		SDL_SetRenderDrawColor(renderer, STATE.caret.color.r,STATE.caret.color.g,STATE.caret.color.b, STATE.caret.color.a);
		SDL_RenderFillRect(renderer, &STATE.caret.rect);
	}
	

	SDL_RenderPresent(renderer);
}

void update_rows(SDL_Renderer* renderer) {
	int y = STATE.colHeight;
	bool exceededScreenHeight = false;

	STATE.rows.clear();
	int i = 1;

	while (!exceededScreenHeight) {
		Row row{};

		SDL_Rect rect{};
		rect.w = STATE.rowWidth;
		rect.h = STATE.rowHeight;
		rect.x = 0;
		rect.y = y;
		row.rect = rect;

		std::string s = std::to_string(i);
		row.content = s;
		SDL_Surface* text = TTF_RenderText_Blended(STATE.FONT, s.c_str(), STATE.fontColor);

		row.textRect = { rect.x + ((row.rect.w - text->w) / 2), rect.y + ((row.rect.h - text->h) / 2), text->w, text->h };
		
		row.textTexture = SDL_CreateTextureFromSurface(renderer, text);

		y += STATE.rowHeight;
		exceededScreenHeight = (rect.y + STATE.rowHeight) >= STATE.screenHeight;

		STATE.rows.push_back(row);
		i++;
	}
}

void update_columns(SDL_Renderer* renderer) {
	int x = STATE.rowWidth;
	bool exceededScreenWidth = false;

	STATE.columns.clear();
	int i = 0;
	while (!exceededScreenWidth) {
		Column col{};

		SDL_Rect rect{};
		rect.w = STATE.colWidth;
		rect.h = STATE.colHeight;
		rect.x = x;
		rect.y = 0;
		col.rect = rect;

		std::string s(1, char(65 + i));
		col.content = s;
		SDL_Surface* text = TTF_RenderText_Blended(STATE.FONT, s.c_str(), STATE.fontColor);

		col.textRect = { rect.x + ((col.rect.w - text->w) / 2), rect.y + ((col.rect.h - text->h) / 2), text->w, text->h };
		col.textTexture = SDL_CreateTextureFromSurface(renderer, text);

		x += STATE.colWidth;
		exceededScreenWidth = (rect.x + STATE.colWidth) >= STATE.screenWidth;

		STATE.columns.push_back(col);
		i++;
	}

}

void update_cells() {
	int y = STATE.colHeight, x = STATE.rowWidth;
	bool screenIsFilled = false;
	STATE.cells.clear();

	uint16_t i = 0;
	int cellsInRow = 0;
	int observedNumberOfRows = 0;
	while (true)
	{
		SDL_Rect cellRect{};
		cellRect.w = STATE.colWidth;
		cellRect.h = STATE.rowHeight;

		Cell cell{};
		
		cell.index = i;

		if (x >= STATE.screenWidth) {
			y += STATE.rowHeight;
			x = STATE.rowWidth;
			assert(cellsInRow == STATE.columns.size());
			cellsInRow = 0;
			observedNumberOfRows += 1;
		}

		cellRect.y = y;
		cellRect.x = x;

		cell.rect = cellRect;

		screenIsFilled = y >= STATE.screenHeight;
		if (screenIsFilled) break;

		STATE.cells.push_back(cell);
		i++;
		cellsInRow++;
		x += STATE.colWidth;
	}
	int numberOfCols = STATE.columns.size();
	int numberOfRows = STATE.rows.size();
	int expectedNumberOfCells = numberOfCols * numberOfRows;
	assert(observedNumberOfRows == numberOfRows);
	assert(expectedNumberOfCells == STATE.cells.size());
}

void update(SDL_Renderer* renderer) {
	update_rows(renderer);
	update_columns(renderer);
	update_cells();
}

void handleEvents(SDL_Renderer* renderer) {
	SDL_Event e;

	SDL_WaitEvent(&e);

	Mode* currentMode = STATE.getCurrentMode();
	currentMode->handleEvents(renderer, &e, &STATE);
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

	STATE.selectedCell = &STATE.cells[0];

	while (!STATE.shouldQuit)
	{
		handleEvents(renderer);

		if (STATE.shouldUpdate) {
			update(renderer);
			STATE.shouldUpdate = false;
		}

		render(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();

	return 0;
}
