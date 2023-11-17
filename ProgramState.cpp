#include "ProgramState.h"
#include <string>

Mode* ProgramState::getCurrentMode() 
{
	for (Mode* mode : _modes) 
	{
		if (mode->type == currentMode)
			return mode;
	}

	assert(false);
}

void ProgramState::moveCaret(int pos)
{
	if (pos <= 0) {
		SDL_Rect cellTextRect = selectedCell->contentRect;
		caret.rect.x = cellTextRect.x;
		caret.rect.y = cellTextRect.y;
		caret.pos = 0;
		return;
	}

	if (pos > selectedCell->content.length()) return;

	int charW, charH;
	std::string cellText = selectedCell->content;
	const int caretCurrentPos = caret.pos;

	int posToTake = pos;

	if (pos > caret.pos) posToTake = caret.pos;

	std::string character(1, cellText[posToTake]);

	if (TTF_SizeText(font, character.c_str(), &charW, &charH)) {
		printf("Failed to determing size of individual char\n");
		return;
	}

	if (pos > caretCurrentPos)
		caret.rect.x += charW;
	else
		caret.rect.x -= charW;

	caret.pos = pos;
}

void ProgramState::moveCaretToStartOfSelectedCell() {
	caret.rect.x = selectedCell->rect.x;
	caret.rect.y = selectedCell->rect.y;
	caret.pos = 0;
}

void ProgramState::moveCaretToEndOfSelectedCellText() {
	caret.rect.x = selectedCell->contentRect.x + selectedCell->contentRect.w;
	caret.rect.y = selectedCell->contentRect.y;
	caret.pos = selectedCell->content.length();
}

Cell* ProgramState::getCellToThe(Cell* cell, Direction direction) {
	int numberOfColumns = columns.size();
	int numberOfRows = rows.size();

	int cellIndex = 0;
	switch (direction)
	{
		case Left:
			cellIndex = cell->index - 1;
			break;
		case Right:
			cellIndex = cell->index + 1;
			break;
		case Up:
			cellIndex = cell->index - numberOfColumns;
			break;
		case Down:
			cellIndex = cell->index + numberOfColumns;
			break;
	}

	if (cellIndex < 0 || cellIndex >= cells.size()) return NULL;

	return &cells[cellIndex];
}

std::string ProgramState::getCellPosLabel(Cell* cell){
	int cellIndex = cell->index;

	int numberOfCols = columns.size();
	int numberOfRows = rows.size();

	int col = cellIndex % numberOfCols;
	int row = cellIndex / numberOfCols;

	std::string colLabel = columns[col].content;
	std::string rowLabel = rows[row].content;

	return (colLabel + rowLabel);
}