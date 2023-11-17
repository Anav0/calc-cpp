#include "ProgramState.h"
#include <string>

Mode* ProgramState::getCurrentMode() 
{
	for (Mode* mode : _modes) 
	{
		if (mode->type == currentMode)
			return mode;
	}

	return NULL;
}

void ProgramState::moveCaret(int pos)
{
	Cell* cell = selectedCell;

	if (pos <= 0) {
		SDL_Rect cellTextRect = cell->contentRect;
		caret.rect.x = cellTextRect.x;
		caret.rect.y = cellTextRect.y;
		caret.pos = 0;
		return;
	}

	if (pos > strlen(cell->content)) return;

	int charW, charH;
	const char* cellText = cell->content;
	const int caretCurrentPos = caret.pos;

	int posToTake = pos;

	if (pos > caret.pos) posToTake = caret.pos;

	std::string character(1, cellText[posToTake]); //Note(Igor): this sucks

	if (TTF_SizeText(FONT, character.c_str(), &charW, &charH)) {
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
	caret.pos = strlen(selectedCell->content);
}

Cell* ProgramState::getCellToThe(Cell* cell, Direction direction) {
	int numberOfColumns = columns.size();
	int numberOfRows = rows.size();

	int index;
	for (index = 0; index < cells.size(); index++) {
		if (cell == &cells[index]) {
			break;
		}
	}

	int newIndex = 0;
	switch (direction)
	{
		case Left:
			newIndex = index - 1;
			break;
		case Right:
			newIndex = index + 1;
			break;
		case Up:
			newIndex = index - numberOfColumns - 1;
			break;
		case Down:
			newIndex = index + numberOfColumns + 1;
			break;
	}

	if (newIndex < 0 || newIndex > cells.size()) return NULL;

	return &cells[newIndex];
}

std::string ProgramState::getCellPosLabel(Cell* cell){
	int cellIndex = NULL;

	for (int i = 0; i < sizeof(cells); i++) {
		if (&cells[i] == cell) {
			cellIndex = i;
			break;
		}
	}

	assert(cellIndex != NULL);

	int numberOfCols = sizeof(columns);
	int numberOfRows = sizeof(rows);

	int col = numberOfCols % cellIndex;
	int row = numberOfRows % cellIndex;

	std::string colLabel = columns[col].content;
	std::string rowLabel = rows[row].content;

	return (colLabel + rowLabel);
}