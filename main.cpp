#include <stdio.h>
#include <stdbool.h>

#include <SDL.h>
#include <SDL_image.h>

// Define MAX and MIN macros
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

// Define screen dimensions
#define SCREEN_WIDTH    1024
#define SCREEN_HEIGHT   720

// Define Tunisia flag image path
#define TN_FLAG_IMAGE_PATH "assets/image.png"

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

int main(int argc, char* argv[])
{
	SDL_Renderer* renderer = 0;
	SDL_Window* window = 0;

	if (!init(&window, &renderer)) {
		return 0;
	}

	SDL_Texture* imageTexture = IMG_LoadTexture(renderer, TN_FLAG_IMAGE_PATH);
	if (!imageTexture)
	{
		printf("Unable to load image '%s'!\n"
			"SDL_image Error: %s", TN_FLAG_IMAGE_PATH, IMG_GetError());
		return false;
	}

	int w, h;
	SDL_QueryTexture(imageTexture, NULL, NULL, &w, &h);

	SDL_Rect imageReact{};
	imageReact.w = w;
	imageReact.h = h;
	imageReact.x = SCREEN_WIDTH / 2 - imageReact.w / 2;
	imageReact.y = SCREEN_HEIGHT / 2 - imageReact.h / 2;

	bool shouldQuit = false;

	while (!shouldQuit)
	{
		SDL_Event e;

		SDL_WaitEvent(&e);

		if (e.type == SDL_QUIT)
			shouldQuit = true;

		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, imageTexture, NULL, &imageReact);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();

	return 0;
}
