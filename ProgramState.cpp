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