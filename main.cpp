#include <stdio.h>
#include <stdbool.h>

#include <SDL.h>
#include <SDL_image.h>

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#define SCREEN_WIDTH    1024
#define SCREEN_HEIGHT   720

#define TN_FLAG_IMAGE_PATH "assets/image.png"

struct Cell {
	//SDL_Texture* texture;
	SDL_Rect rect;
};

int init(SDL_Window** window, SDL_Renderer** renderer)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL2 could not be initialized!\n" "SDL2 Error: %s\n", SDL_GetError());
		return 0;
	}

	int flags = IMG_INIT_PNG;
	if ((IMG_Init(flags) & flags) != flags) {
		printf("SDL2_image could not be initialized with PNG support!\n"
			"SDL2_image Error: %s\n", IMG_GetError());
		return 0;
	}

	*window = SDL_CreateWindow("Sokoban", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (!window)
	{
		printf("Window could not be created!\n" "SDL_Error: %s\n", SDL_GetError());
		return 0;
	}
	*renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
	{
		printf("Renderer could not be created!\n"
			"SDL_Error: %s\n", SDL_GetError());

		return 0;
	}
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

int main(int argc, char* argv[])
{
	SDL_Renderer* renderer = 0;
	SDL_Window* window = 0;
	bool shouldQuit = false;

	if (!init(&window, &renderer)) {
		return 0;
	}

	const int n = 200;
	Cell cells[n];
	int y = 0, x = 0;
	for (size_t i = 0; i < n; i++)
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
		cells[i] = cell;
	}

	while (!shouldQuit)
	{
		SDL_Event e;

		SDL_WaitEvent(&e);

		if (e.type == SDL_QUIT)
			shouldQuit = true;

		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 245, 141, 86, 0xFF); //orange
		
		for (Cell cell : cells)
		{
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
