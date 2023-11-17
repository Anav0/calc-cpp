#pragma once
#include <string>

#include <SDL_rect.h>
#include <SDL.h>
#include <SDL_ttf.h>

#define MAX_TEXT_LEN 256

enum ModeType {
	View,
	Edit,
	Expr
};

enum Direction {
	Left,
	Right,
	Up,
	Down
};

enum CaretType {
	Vertical
};

struct Caret {
	SDL_Rect rect;
	SDL_Color color = { 45, 124, 238, 0xFF };

	uint8_t frequency;
	uint16_t pos;

	CaretType type;
};

struct Cell {
	SDL_Rect rect;

	SDL_Rect contentRect;
	SDL_Texture* contentTexture;
	char content[MAX_TEXT_LEN];

	uint16_t index;
};

struct Column {
	SDL_Rect rect;

	SDL_Rect textRect;
	SDL_Texture* textTexture;

	std::string content;
};

struct Row {
	SDL_Rect rect;

	SDL_Rect textRect;
	SDL_Texture* textTexture;

	std::string content;
};

inline void updateCellContentTexture(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, Cell* cell) {
	SDL_Surface* text = TTF_RenderText_Blended(font, cell->content, color);

	if (text == NULL) {
		cell->contentTexture = NULL;
		return;
	}

	cell->contentRect = { cell->rect.x, cell->rect.y + ((cell->rect.h - text->h) / 2), text->w, text->h };
	
	cell->contentTexture = SDL_CreateTextureFromSurface(renderer, text);
}

inline void removeAt(int index, char arr[MAX_TEXT_LEN]) {
	for (int i = index; i < MAX_TEXT_LEN-1; i++) {
		arr[i] = arr[i + 1];
		arr[i + 1] = '\0';
	}
}