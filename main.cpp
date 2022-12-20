#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_timer.h>

SDL_Window* gWindow;
SDL_Surface* gScreenSurface;
SDL_Surface* gHelloWorld;
bool gShouldQuit = false;

void cleanup()
{
	SDL_FreeSurface(gHelloWorld);
	gHelloWorld = NULL;

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	SDL_Quit();
}

void printKey(SDL_Event e) {
	printf("%s\n", SDL_GetKeyName(e.key.keysym.sym));
}

void handleEvents()
{
	SDL_Event e;

	while (SDL_PollEvent(&e) != 0)
	{
		switch (e.type) 
		{
			case SDL_QUIT:
				gShouldQuit = true;
				break;
			case SDL_KEYDOWN:
				printKey(e);
				break;
		}
	}
}

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
	}

	gWindow = SDL_CreateWindow("Sokoban", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 720, 0);

	if (gWindow == NULL)
		printf("Failed to create a window: %s\n", SDL_GetError());

	gScreenSurface = SDL_GetWindowSurface(gWindow);

	while (!gShouldQuit)
	{
		handleEvents();
	}

	cleanup();
	return 0;
}