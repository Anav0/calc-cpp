#include "Mode.h"
#include <string>

#include "ProgramState.h"
#include "Gui.h"

void Mode::handleEvents(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state) {
	switch (e->type) {
	case SDL_QUIT:
		state->shouldQuit = true;
		break;

	case SDL_KEYDOWN:
		handleKeydownEvent(renderer, e, state);
		break;

	case SDL_TEXTINPUT:
		handleTextInput(renderer, e, state);
		break;

	case SDL_MOUSEMOTION:
		handleMouseMotion(renderer, e, state);
		break;

	case SDL_MOUSEBUTTONDOWN:
		handleMouseButtonDown(renderer, e, state);
		break;

	case SDL_WINDOWEVENT:
		handleWindowEvent(renderer, e, state);
		break;
	}
}

void Mode::handleMouseMotion(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state) {
	state->mousePos.x = e->motion.x;
	state->mousePos.y = e->motion.y;
}

void Mode::handleWindowEvent(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state) {
	switch (e->window.event) {
	case SDL_WINDOWEVENT_RESIZED:
		SDL_Log("Window %d resized to %dx%d", e->window.windowID, e->window.data1,
			e->window.data2);
		state->screenWidth = e->window.data1;
		state->screenHeight = e->window.data2;

		state->shouldUpdate = true;
	}
}

void Mode::handleKeydownEvent(SDL_Renderer*, SDL_Event*, ProgramState*) {};
void Mode::handleMouseButtonDown(SDL_Renderer*, SDL_Event*, ProgramState*) {};
void Mode::handleTextInput(SDL_Renderer*, SDL_Event*, ProgramState*) {};
void Mode::onSwitchTo(SDL_Renderer*, ProgramState*) {};

// ===============
// == EDIT MODE ==
// ===============

void EditMode::onSwitchTo(SDL_Renderer* renderer, ProgramState* state) {

	if (state->selectedCell->formula != "") {
		state->selectedCell->content = state->selectedCell->formula;
		state->selectedCell->showEvaluation(renderer, state->font, state->fontColor, state->cellPadding);
	}

	if (state->selectedCell->content.length() == 0)
		state->moveCaretToStartOfSelectedCell();
	else
		state->moveCaretToEndOfSelectedCellText();
}

void EditMode::moveToExprMode(SDL_Renderer* renderer, ProgramState* state,
	Direction dir) {
	Cell* newCell = state->getCellToThe(state->selectedCell, dir);

	if (newCell == NULL)
		return;

	state->subjectCell = state->selectedCell;
	state->selectedCell = newCell;
	state->subjectInsertPos = 1; // Note(Igor): not always correct index
	state->switchMode(renderer, Expr);
}

void EditMode::handleKeydownEvent(SDL_Renderer* renderer, SDL_Event* e,
	ProgramState* state) {
	switch (e->key.keysym.scancode) {
	case SDL_SCANCODE_ESCAPE:
		if (state->evaluate(renderer, state->selectedCell))
			state->selectedCell->showEvaluation(renderer, state->font, state->fontColor, state->cellPadding);
		else
			state->selectedCell->showErrorMessage(renderer, state->font, state->fontColor, state->cellPadding);
		state->switchMode(renderer, View);
		break;
	case SDL_SCANCODE_RETURN:
		if (state->evaluate(renderer, state->selectedCell))
			state->selectedCell->showEvaluation(renderer, state->font, state->fontColor, state->cellPadding);
		else
			state->selectedCell->showErrorMessage(renderer, state->font, state->fontColor, state->cellPadding);

		state->recordCellChange();

		state->selectedCell = state->getCellToThe(state->selectedCell, Down);
		state->switchMode(renderer, View);
		break;
	case SDL_SCANCODE_HOME:
		state->moveCaret(0);
		break;
	case SDL_SCANCODE_END:
		state->moveCaretToEndOfSelectedCellText();
		break;
	case SDL_SCANCODE_DELETE:
		if (state->caret.pos == state->selectedCell->content.length())
			return;

		state->recordCellChange();

		state->selectedCell->content.erase(state->caret.pos, 1);

		if (state->selectedCell->formula != "")
			state->selectedCell->formula = state->selectedCell->content;

		state->selectedCell->showEvaluation(
			renderer, state->font, state->fontColor, state->cellPadding);
		break;
	case SDL_SCANCODE_BACKSPACE:
		if (state->caret.pos == 0)
			return;

		state->recordCellChange();

		state->moveCaret(state->caret.pos - 1);
		state->selectedCell->content.erase(state->caret.pos, 1);

		if (state->selectedCell->formula != "")
			state->selectedCell->formula = state->selectedCell->content;

		state->selectedCell->showEvaluation(
			renderer, state->font, state->fontColor, state->cellPadding);

		break;
	case SDL_SCANCODE_DOWN:
		if (state->selectedCell->content[0] == '=')
			moveToExprMode(renderer, state, Down);
	case SDL_SCANCODE_UP:
		if (state->selectedCell->content[0] == '=')
			moveToExprMode(renderer, state, Up);
		break;
	case SDL_SCANCODE_LEFT:
		state->moveCaret(state->caret.pos - 1);
		break;
	case SDL_SCANCODE_RIGHT:
		state->moveCaret(state->caret.pos + 1);
		break;
	}
}

void EditMode::handleMouseButtonDown(SDL_Renderer* renderer, SDL_Event* e,
	ProgramState* state) {
	state->switchMode(renderer, View);
}

void EditMode::handleTextInput(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state) {

	if (state->selectedCell->content.length() + SDL_strlen(e->text.text) < MAX_TEXT_LEN) {

		state->selectedCell->content.insert(state->caret.pos, e->text.text);

		state->moveCaret(state->caret.pos + 1);

		state->selectedCell->showEvaluation(
			renderer, state->font, state->fontColor, state->cellPadding);
	}
	else {
		printf("Input is too long!\n");
	}
}

// ===============
// == VIEW MODE ==
// ===============

void ViewMode::handleKeydownEvent(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state) {
	if(Gui::activeElementId > 0)
		return;

	switch (e->key.keysym.scancode) {
	case SDL_SCANCODE_S:
		if (e->key.keysym.mod & KMOD_LCTRL)
			state->changes.clear();
		break;
	case SDL_SCANCODE_Z:
		if (e->key.keysym.mod & KMOD_LCTRL)
			state->changes.undo(state->cells);
		break;
	case SDL_SCANCODE_RETURN:
		state->switchMode(renderer, Edit);
		break;
	case SDL_SCANCODE_DELETE:
		state->selectedCell->content = "";
		state->selectedCell->showEvaluation(
			renderer, state->font, state->fontColor, state->cellPadding);
		break;
	case SDL_SCANCODE_DOWN:
		navigate(renderer, Direction::Down, state);
		break;
	case SDL_SCANCODE_UP:
		navigate(renderer, Direction::Up, state);
		break;
	case SDL_SCANCODE_LEFT:
		navigate(renderer, Direction::Left, state);
		break;
	case SDL_SCANCODE_RIGHT:
		navigate(renderer, Direction::Right, state);
		break;
	}
}

void ViewMode::handleTextInput(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state) {
	
	if (Gui::activeElementId) return;

	if (SDL_strlen(e->text.text) < MAX_TEXT_LEN) {
		state->selectedCell->content = *e->text.text;
		state->selectedCell->showEvaluation(
			renderer, state->font, state->fontColor, state->cellPadding);
		state->moveCaretToEndOfSelectedCellText();
		state->switchMode(renderer, Edit);
	}
	else {
		printf("Input is too big!\n");
	}

	if (state->selectedCell->content[0] == '=') {
		state->subjectInsertPos = 1;
		state->subjectCell = state->selectedCell;
		state->switchMode(renderer, Expr);
	}
}

void ViewMode::handleMouseButtonDown(SDL_Renderer* renderer, SDL_Event* e,
	ProgramState* state) {
	switch (e->button.button) {
	case SDL_BUTTON_LEFT:
		//SDL_StopTextInput();

		for (Cell& cell : state->cells) {
			if (SDL_PointInRect(&state->mousePos, &cell.rect)) {
				state->selectedCell = &cell;
				SDL_StartTextInput();
				SDL_SetTextInputRect(&cell.rect);
			}
		}
		break;
	}
}

void ViewMode::navigate(SDL_Renderer* renderer, Direction direction,
	ProgramState* state) {
	Cell* cell = state->getCellToThe(state->selectedCell, direction);

	if (cell != NULL) {
		state->selectedCell = cell;
	}
}

// ===============
// == EXPR MODE ==
// ===============

void ExprMode::onSwitchTo(SDL_Renderer* renderer, ProgramState* state) {
	// evaluate(state->selectedCell, state->cells, state->columns);
	//
	// if(state->subjectCell != NULL)
	//	evaluate(state->subjectCell, state->cells, state->columns);
	//
	// if (state->selectedCell->formula != "") {
	//	selectedCellContent = state->selectedCell->formula;
	//	state->selectedCell->updateContentTexture(renderer, state->font,
	//state->fontColor, state->cellPadding);
	//}
}

void ExprMode::addOperator(SDL_Renderer* renderer, char sign,
	ProgramState* state) {
	state->subjectCell->content += sign;
	state->subjectInsertPos = state->subjectCell->content.length();
	state->subjectCell->showEvaluation(
		renderer, state->font, state->fontColor, state->cellPadding);
}

void ExprMode::navigate(SDL_Renderer* renderer, Direction dir,
	ProgramState* state) {

	Cell* cell = state->getCellToThe(state->selectedCell, dir);

	if (cell == NULL)
		return;

	state->selectedCell = cell;

	std::string label = state->getCellPosLabel(cell);

	if (state->subjectCell->content.length() < state->subjectInsertPos)
		state->subjectCell->content += label;
	else
		state->subjectCell->content.replace(state->subjectInsertPos, label.length(),
			label);

	state->subjectCell->showEvaluation(
		renderer, state->font, state->fontColor, state->cellPadding);
}

void ExprMode::handleKeydownEvent(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state) {
	switch (e->key.keysym.scancode) {
	case SDL_SCANCODE_ESCAPE:
	case SDL_SCANCODE_RETURN:

		if (state->evaluate(renderer, state->subjectCell))
			state->subjectCell->showEvaluation(renderer, state->font, state->fontColor, state->cellPadding);
		else
			state->subjectCell->showErrorMessage(renderer, state->font, state->fontColor, state->cellPadding);

		state->subjectCell = NULL;
		state->subjectInsertPos = 1;
		state->switchMode(renderer, View);
		break;
	case SDL_SCANCODE_DOWN:
		navigate(renderer, Direction::Down, state);
		break;
	case SDL_SCANCODE_UP:
		navigate(renderer, Direction::Up, state);
		break;
	case SDL_SCANCODE_LEFT:
		navigate(renderer, Direction::Left, state);
		break;
	case SDL_SCANCODE_RIGHT:
		navigate(renderer, Direction::Right, state);
		break;
	case SDL_SCANCODE_KP_PLUS:
		addOperator(renderer, '+', state);
		break;
	case SDL_SCANCODE_KP_MINUS:
		addOperator(renderer, '-', state);
		break;
	case SDL_SCANCODE_KP_MULTIPLY:
		addOperator(renderer, '*', state);
		break;
	}
}
