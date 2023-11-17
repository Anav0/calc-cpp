#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <string>
#include "Align.h"

class Gui {
private:
	static SDL_Renderer* renderer;
	static TTF_Font* defaultFont;
	static SDL_Point mousePos;
	static bool clicked;
	Gui() {};

public:
	static void init(SDL_Renderer*, TTF_Font*);
	static void endOfLoop();
	static void drawText(int, int, SDL_Color, std::string, TTF_Font*);
	static void drawText(int, int, SDL_Color, std::string);
	static bool drawBtn(int, int, SDL_Color, std::string);
	static void events(SDL_Event*);
};