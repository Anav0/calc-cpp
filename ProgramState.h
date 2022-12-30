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
	int rowWidth = 40;
	int rowHeight = 20;

	int colWidth = 100;
	int colHeight = 20;

	int screenWidth = 1336;
	int screenHeight = 768;

	bool shouldQuit;
	bool shouldUpdate;

	ModeType currentMode = View;

	SDL_Point mousePos;

	Sint32 cursor;
	SDL_Color fontColor = { 0, 0, 0, 0 };
	TTF_Font* FONT;

	Caret caret;

	Cell* selectedCell;
	std::vector<Cell>   cells;
	std::vector<Column> columns;
	std::vector<Row>    rows;

	ProgramState() {
		_modes.push_back(new EditMode());
		_modes.push_back(new ViewMode());

		SDL_Rect caretRect{};
		caretRect.x = 0;
		caretRect.y = 0;
		caretRect.w = 2;
		caretRect.h = colHeight * 0.80;
		caret.rect = caretRect;
	}

	Mode* getCurrentMode();
	void moveCaret(int pos);
	void moveCaretToEndOfSelectedCellText();
	void moveCaretToStartOfSelectedCell();
};
