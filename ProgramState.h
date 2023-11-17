#pragma once

#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>

#include "Mode.h"

struct ProgramState
{
private:
	std::vector<Mode*> _modes;

public:
	int ROW_W = 40;
	int ROW_H = 20;

	int COL_W = 100;
	int COL_H = 20;

	int SCREEN_WIDTH = 1336;
	int SCREEN_HEIGHT = 768;

	bool SHOULD_QUIT;
	bool shouldUpdate;

	ModeType CURRENT_MODE = View;

	SDL_Point MOUSE_POS;

	Sint32 cursor;
	SDL_Color FONT_COLOR = { 0, 0, 0, 0 };
	TTF_Font* FONT;

	Cell* SELECTED_CELL;
	std::vector<Cell>   CELLS;
	std::vector<Column> COLUMNS;
	std::vector<Row>    ROWS;

	ProgramState() {
		_modes.push_back(new EditMode());
		_modes.push_back(new ViewMode());
	}

	Mode* getCurrentMode();
};
