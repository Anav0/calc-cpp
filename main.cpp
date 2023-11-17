#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdbool.h>
#include <vector>
#include <cassert>
#include <format>
#include <string>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "Base.h"
#include "ProgramState.h"
#include "Gui.h"


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
	STATE.font = TTF_OpenFont(font_path, 14);
	Gui::init(*renderer, STATE.font);

	if (!STATE.font) {
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

	SDL_Color modeColor = STATE.getCurrentMode()->color;

	for (const Cell& cell : STATE.cells)
	{
		SDL_SetRenderDrawColor(renderer, 192, 192, 192, 0xFF);

		if (SDL_PointInRect(&STATE.mousePos, &cell.rect))
			SDL_SetRenderDrawColor(renderer, STATE.hoverColor.r, STATE.hoverColor.g, STATE.hoverColor.b, STATE.hoverColor.a);

		if (STATE.subjectCell == &cell)
			SDL_SetRenderDrawColor(renderer, STATE.subjectColor.r, STATE.subjectColor.g, STATE.subjectColor.b, STATE.subjectColor.a);

		if (STATE.selectedCell == &cell)
			SDL_SetRenderDrawColor(renderer, modeColor.r, modeColor.g, modeColor.b, modeColor.a);

		if (cell.Err != Ok)
			SDL_SetRenderDrawColor(renderer, 229, 65, 51, 0); //Note(Igor): extract into config file

		SDL_RenderDrawRect(renderer, &cell.rect);

		if (cell.contentTexture != NULL)
			SDL_RenderCopy(renderer, cell.contentTexture, NULL, &cell.contentRect);
	}

	int colIndex = 0;
	for (const Column& column : STATE.columns)
	{
		SDL_SetRenderDrawColor(renderer, 248, 249, 250, 0xFF);
		SDL_RenderFillRect(renderer, &column.rect);
		SDL_SetRenderDrawColor(renderer, 67, 66, 61, 0xFF);
		SDL_RenderDrawRect(renderer, &column.rect);

		if (colIndex == STATE.selectedCell->colIndex) {
			SDL_SetRenderDrawColor(renderer, modeColor.r, modeColor.g, modeColor.b, modeColor.a);
			SDL_RenderFillRect(renderer, &column.rect);
		}

		SDL_RenderCopy(renderer, column.textTexture, NULL, &column.textRect);
		colIndex++;
	}

	int rowIndex = 0;
	for (const Row& row : STATE.rows)
	{
		SDL_SetRenderDrawColor(renderer, 248, 249, 250, 0xFF);
		SDL_RenderFillRect(renderer, &row.rect);
		SDL_SetRenderDrawColor(renderer, 67, 66, 61, 0xFF);
		SDL_RenderDrawRect(renderer, &row.rect);

		if (rowIndex == STATE.selectedCell->rowIndex) {
			SDL_SetRenderDrawColor(renderer, modeColor.r, modeColor.g, modeColor.b, modeColor.a);
			SDL_RenderFillRect(renderer, &row.rect);
		}

		SDL_RenderCopy(renderer, row.textTexture, NULL, &row.textRect);
		rowIndex++;
	}

	if (STATE.getCurrentMode()->type == Edit) {
		SDL_SetRenderDrawColor(renderer, STATE.caret.color.r, STATE.caret.color.g, STATE.caret.color.b, STATE.caret.color.a);
		SDL_RenderFillRect(renderer, &STATE.caret.rect);
	}

	auto numberOfChanges = STATE.getNumberOfChanges();
	if (numberOfChanges > 0) {
		Gui::drawText(50, STATE.screenHeight - 50, STATE.fontColor, std::to_string(numberOfChanges) + " unsaved changes");
		if (Gui::drawBtn(100, 100, STATE.fontColor, "CLICK")) {

		}
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
		SDL_Surface* text = TTF_RenderText_Blended(STATE.font, s.c_str(), STATE.fontColor);

		row.textRect.w = text->w;
		row.textRect.h = text->h;
		center(&row.rect, &row.textRect);

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
		SDL_Surface* text = TTF_RenderText_Blended(STATE.font, s.c_str(), STATE.fontColor);

		col.textRect.w = text->w;
		col.textRect.h = text->h;
		center(&col.rect, &col.textRect);

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
	int rowIndex = 0;
	int columnIndex = 0;
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
			rowIndex++;
			columnIndex = 0;
		}

		cellRect.y = y;
		cellRect.x = x;

		cell.rect = cellRect;
		cell.colIndex = columnIndex;
		cell.rowIndex = rowIndex;

		screenIsFilled = y >= STATE.screenHeight;
		if (screenIsFilled) break;

		STATE.cells.push_back(cell);
		i++;
		cellsInRow++;
		columnIndex++;
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

	Gui::events(&e);
	Mode* currentMode = STATE.getCurrentMode();
	currentMode->handleEvents(renderer, &e, &STATE);
}

int main(int argc, char* argv[])
{
	SDL_Renderer* renderer;
	SDL_Window* window;

	if (!init(&window, &renderer)) {
		return 1;
	}

	SDL_StartTextInput();

	update(renderer);

	STATE.selectedCell = &STATE.cells[0];

	while (!STATE.shouldQuit)
	{
		handleEvents(renderer);

		if (STATE.shouldUpdate) {
			update(renderer);
			STATE.reset(); //Note(Igor): temperary solution
			STATE.shouldUpdate = false;
		}

		render(renderer);

		Gui::endOfLoop();
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();

	return 0;
}
