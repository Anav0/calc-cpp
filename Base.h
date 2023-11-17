#pragma once
#include <string>
#include <unordered_set>
#include <cassert>

#include <SDL_rect.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include "Align.h"

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

enum CellError {
	Ok,
	SelfReference,
	NaN,
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
	std::string content;
	std::string formula;

	int index;
	int colIndex;
	int rowIndex;

	CellError Err = Ok;

	std::unordered_set<Cell*> dependant_cells;

	void showErrorMessage(SDL_Renderer*, TTF_Font*, SDL_Color, int[4]);
	void showFormula(SDL_Renderer*, TTF_Font*, SDL_Color, int[4]);
	void showEvaluation(SDL_Renderer*, TTF_Font*, SDL_Color, int[4]);
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