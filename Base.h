#pragma once

#include <SDL_rect.h>
#include <SDL.h>
#include <SDL_ttf.h>

#define MAX_TEXT_LEN 256

enum ModeType {
	View,
	Edit,
};

enum Direction {
	Left,
	Right,
	Up,
	Down
};

struct Cell {
	SDL_Rect rect;

	SDL_Rect contentRect;
	SDL_Texture* contentTexture;
	char content[MAX_TEXT_LEN];
};

struct Column {
	SDL_Rect rect;

	SDL_Rect textRect;
	SDL_Texture* textTexture;
};

struct Row {
	SDL_Rect rect;

	SDL_Rect textRect;
	SDL_Texture* textTexture;
};

inline void updateCellContentTexture(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, Cell* cell) {
	SDL_Surface* text = TTF_RenderText_Blended(font, cell->content, color);

	if (text == NULL) {
		cell->contentTexture = NULL;
		return;
	}

	cell->contentRect = { cell->rect.x + ((cell->rect.w - text->w) / 2), cell->rect.y + ((cell->rect.h - text->h) / 2), text->w, text->h };
	cell->contentTexture = SDL_CreateTextureFromSurface(renderer, text);
}