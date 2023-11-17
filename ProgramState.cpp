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
		caret.rect.x = selectedCell->rect.x + cellPadding[0];
		caret.rect.y = selectedCell->rect.y + ((selectedCell->rect.h - selectedCell->rect.h) / 2);
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
	caret.rect.x = selectedCell->rect.x + cellPadding[0];
	caret.rect.y = selectedCell->rect.y + ((selectedCell->rect.h - caret.rect.h) / 2);
	caret.pos = 0;
}

void ProgramState::moveCaretToEndOfSelectedCellText() {
	if (selectedCell->content == "") return;

	caret.rect.x = selectedCell->contentRect.x + selectedCell->contentRect.w;
	caret.rect.y = selectedCell->rect.y + ((selectedCell->rect.h - caret.rect.h) / 2);
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

std::string ProgramState::getCellPosLabel(Cell* cell) {
	std::string colLabel = columns[cell->colIndex].content;
	std::string rowLabel = rows[cell->rowIndex].content;

	return (colLabel + rowLabel);
}

void ProgramState::switchMode(SDL_Renderer* renderer, ModeType modeType) {

	currentMode = modeType;

	Mode* mode = getCurrentMode();

	mode->onSwitchTo(renderer, this);
}

bool ProgramState::evaluate(SDL_Renderer* renderer, Cell* cell) {

	bool haveFormula = cell->formula.length() > 0;
	bool haveExprInContent = cell->content.length() > 0 && cell->content[0] == '=';
	bool doNotEval = !haveFormula && !haveExprInContent;

	if (doNotEval) {
		for (Cell* dependant_cell : cell->dependant_cells) {
			if (evaluate(renderer, dependant_cell))
				dependant_cell->showEvaluation(renderer, font, fontColor, cellPadding);
		}

		return false;
	};

	std::string expr;
	if (haveExprInContent) {
		expr = cell->content;
		cell->formula = cell->content;
	}
	else
		expr = cell->formula;

	expr = expr.erase(0, 1); // Removes '='

	std::regex labelsRgx(R"(([A-Z])+(\d)+)");
	std::smatch match;

	bool haveCellPosToReplace = true;

	while (haveCellPosToReplace) {
		std::string exprCopy = expr;
		auto regexIterStart  = std::sregex_iterator(exprCopy.begin(), exprCopy.end(), labelsRgx);
		auto regexIterEnd    = std::sregex_iterator();

		haveCellPosToReplace = false;
		for (std::sregex_iterator i = regexIterStart; i != regexIterEnd; ++i) {

			haveCellPosToReplace = true;

			std::smatch match = *i;
			std::string cellPos = match[0];

			//Note(Igor): this will not work on two and more char columns 
			std::string col = cellPos.substr(0, 1);
			int rowIndex = std::stoi(cellPos.substr(1)) - 1;

			int colIndex = (int)col[0] - 65;
			int cellIndex = (rowIndex * columns.size()) + colIndex;

			//TODO(Igor): COPY
			assert(cellIndex <= cells.size());

			Cell* c = &cells.at(cellIndex);

			//Note(Igor): side effect
			if (c->index == cell->index) {
				cell->Err = SelfReference;
				return false;
			}

			c->dependant_cells.insert(cell);

			std::regex r(cellPos);
			expr = std::regex_replace(expr, r, c->content); //Info(Igor): slooooow
		}
	}

	int err = 0;
	double result = te_interp(expr.c_str(), &err);
	cell->content = std::to_string(result);

	for (Cell* dependant_cell : cell->dependant_cells) {
		if (evaluate(renderer, dependant_cell))
			dependant_cell->showEvaluation(renderer, font, fontColor, cellPadding);
	}

	if (err) {
		cell->Err = NaN;
		return false;
	}
	else {
		cell->Err = Ok;
		return true;
	}
}

void ProgramState::reset() {
	selectedCell = &cells[0];
	
	while (!changes.queque.empty())
		changes.queque.pop_back();
}

void ProgramState::recordCellChange()
{
	if (changes.queque.size() >= MAX_CHANGES)
		changes.queque.pop_back();

	CellChange* cellChange = new CellChange(*selectedCell);
	changes.queque.push_front(cellChange);
}

int ProgramState::getNumberOfChanges()
{
	return changes.length();
}
