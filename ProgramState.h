#pragma once

#include <vector>
#include <cassert>
#include <regex>

#include <SDL.h>
#include <SDL_ttf.h>

#include "Base.h"
#include "tinyexpr.h"

#include "Mode.h"

struct ProgramState
{
private:
	std::vector<Mode*> _modes;
	ModeType currentMode = View;

public:
	int rowWidth = 40;
	int rowHeight = 20;

	int colWidth = 100;
	int colHeight = 20;

	int screenWidth = rowWidth + colWidth * 12;
	int screenHeight = colHeight + rowHeight * 40;

	bool shouldQuit;
	bool shouldUpdate;

	int cellPadding[4] = {10, 10, 0, 0}; // L R T B

	SDL_Point mousePos;

	Sint32 cursor;
	TTF_Font* font;

	SDL_Color fontColor           = { 0, 0, 0, 0 };
	SDL_Color subjectColor        = {255, 188, 71, 0};
	SDL_Color selectedColor       = {38, 87, 82, 0};
	SDL_Color hoverColor          = {38, 87, 82, 0};
	SDL_Color activeColumnColor   = {255,206,69, 0};
	SDL_Color activeRowColor      = {255,206,69, 0};

	Caret caret;

	Cell* selectedCell;
	Cell* subjectCell;

	int subjectInsertPos;

	std::vector<Cell>   cells;
	std::vector<Column> columns;
	std::vector<Row>    rows;

	ProgramState() {
		_modes.push_back(new EditMode());
		_modes.push_back(new ViewMode());
		_modes.push_back(new ExprMode());

		SDL_Rect caretRect{};
		caretRect.x = 0;
		caretRect.y = 0;
		caretRect.w = 2;
		caretRect.h = colHeight * 0.80;
		caret.rect = caretRect;
	}

	Mode* getCurrentMode();

	void switchMode(SDL_Renderer*, ModeType mode);
	
	void moveCaret(int pos);
	void moveCaretToEndOfSelectedCellText();
	void moveCaretToStartOfSelectedCell();

	std::string getCellPosLabel(Cell*);
	Cell* getCellToThe(Cell*, Direction);

	bool evaluate(SDL_Renderer*, Cell*);
};
