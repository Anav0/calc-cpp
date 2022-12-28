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

	std::string character(1, cellText[pos]); //Note(Igor): this sucks

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