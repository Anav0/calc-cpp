#include <stdio.h>
#include <stdbool.h>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

static int SCREEN_WIDTH  = 1024;
static int SCREEN_HEIGHT = 720;
static bool SHOULD_QUIT  = false;

struct Cell {
	SDL_Rect rect;
};

static std::vector<Cell> CELLS;

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

void update_cells() {
	int y = 0, x = 0;
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
			x = 0;
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

void handleMouseButtonDown(SDL_Event* e) {

	switch (e->button.button) {
		case SDL_BUTTON_LEFT:
			printf("Left btn down!\n");
			break;

		case SDL_BUTTON_RIGHT:
			printf("Right btn down!\n");
			break;

		case SDL_BUTTON_MIDDLE:
			printf("Middle btn down!\n");
			break;
	}

}

void handleWindowEvent(SDL_Event* e) {
	switch (e->window.event) {
		case SDL_WINDOWEVENT_RESIZED:
			SDL_Log("Window %d resized to %dx%d", e->window.windowID, e->window.data1, e->window.data2);
			SCREEN_WIDTH = e->window.data1;
			SCREEN_HEIGHT= e->window.data2;
			update_cells();
	}
}

void handleEvents() {
	SDL_Event e;

	SDL_WaitEvent(&e);

	switch (e.type) {
		case SDL_QUIT:
			SHOULD_QUIT = true;
			break;
		case SDL_MOUSEBUTTONDOWN:
			handleMouseButtonDown(&e);
		case SDL_WINDOWEVENT:
			handleWindowEvent(&e);
	}
}

int main(int argc, char* argv[])
{
	SDL_Renderer* renderer = 0;
	SDL_Window*   window   = 0;
	SDL_Point     mouse_position;

	if (!init(&window, &renderer)) {
		return 0;
	}

	std::vector<Cell> cells;
	int y = 0, x = 0;
	bool screenIsNotFilled = true;

	update_cells();

	while (!SHOULD_QUIT)
	{
 		SDL_GetMouseState(&mouse_position.x, &mouse_position.y);

		handleEvents();

		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);

		for (Cell cell : CELLS)
		{
			if (SDL_PointInRect(&mouse_position, &cell.rect)) {
				SDL_SetRenderDrawColor(renderer, 38, 87, 82, 0xFF);
			} else {
				SDL_SetRenderDrawColor(renderer, 192, 192, 192, 0xFF);
			}

			SDL_RenderDrawRect(renderer, &cell.rect);
		}
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();

	return 0;
}
