#include "Mode.h"

#include "ProgramState.h"

void Mode::handleEvents(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state)
{
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
		SDL_Log("Window %d resized to %dx%d", e->window.windowID, e->window.data1, e->window.data2);
		state->screenWidth = e->window.data1;
		state->screenHeight = e->window.data2;

		state->shouldUpdate = true;
	}
}

void Mode::handleKeydownEvent(SDL_Renderer*, SDL_Event*, ProgramState*) {};
void Mode::handleMouseButtonDown(SDL_Renderer*, SDL_Event*, ProgramState*) {};
void Mode::handleTextInput(SDL_Renderer*, SDL_Event*, ProgramState*) {};

// ===============
// == EDIT MODE ==
// ===============

void EditMode::handleKeydownEvent(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state)
{
	switch (e->key.keysym.scancode) {
	case SDL_SCANCODE_ESCAPE:
		state->currentMode = View;
		break;
	case SDL_SCANCODE_RETURN:
		state->currentMode = View;
		break;
	case SDL_SCANCODE_HOME:
		state->moveCaret(0);
		break;
	case SDL_SCANCODE_END:
		state->moveCaretToEndOfSelectedCellText();
		break;
	case SDL_SCANCODE_DELETE:
		if (state->caret.pos == state->selectedCell->content.length()) return;
		state->selectedCell->content.erase(state->caret.pos, 1);
		updateCellContentTexture(renderer, state->FONT, state->fontColor, state->selectedCell);
		break;
	case SDL_SCANCODE_BACKSPACE:
		if (state->caret.pos == 0) return;
		state->moveCaret(state->caret.pos - 1);
		state->selectedCell->content.erase(state->caret.pos, 1);
		updateCellContentTexture(renderer, state->FONT, state->fontColor, state->selectedCell);
		break;
	case SDL_SCANCODE_DOWN:
		break;
	case SDL_SCANCODE_UP:
		break;
	case SDL_SCANCODE_LEFT:
		state->moveCaret(state->caret.pos - 1);
		break;
	case SDL_SCANCODE_RIGHT:
		state->moveCaret(state->caret.pos + 1);
		break;
	}
}

void EditMode::handleMouseButtonDown(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state) {
	state->currentMode = View;
}

void EditMode::handleTextInput(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state) {
	if (state->selectedCell->content.length() + SDL_strlen(e->text.text) < MAX_TEXT_LEN) {

		state->selectedCell->content += *e->text.text;

		state->moveCaret(state->caret.pos + 1);

		updateCellContentTexture(renderer, state->FONT, state->fontColor, state->selectedCell);
	}
	else {
		printf("Input is too big!\n");
	}

	if (state->selectedCell->content[0] == '=') {
		state->subjectInsertPos = 1;
		state->subjectCell = state->selectedCell;
		state->currentMode = Expr;
	}
}

// ===============
// == VIEW MODE ==
// ===============

void ViewMode::handleKeydownEvent(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state)
{
	switch (e->key.keysym.scancode) {
	case SDL_SCANCODE_RETURN:
		if (state->selectedCell->content[0] == '\0') {
			state->moveCaretToStartOfSelectedCell();
		}
		else {
			state->moveCaretToEndOfSelectedCellText();
		}
		state->currentMode = Edit;
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

void ViewMode::handleMouseButtonDown(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state) {
	switch (e->button.button) {
	case SDL_BUTTON_LEFT:
		SDL_StopTextInput();

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

void ViewMode::navigate(SDL_Renderer* renderer, Direction direction, ProgramState* state) {
	Cell* cell = state->getCellToThe(state->selectedCell, direction);

	if (cell != NULL) {
		state->selectedCell = cell;
	}
}

// ===============
// == EXPR MODE ==
// ===============

void ExprMode::navigate(SDL_Renderer* renderer, Direction dir, ProgramState* state)
{
	Cell* cell = state->getCellToThe(state->selectedCell, dir);

	if (cell != NULL) {
		state->selectedCell = cell;

		std::string label = state->getCellPosLabel(cell);

		if(state->subjectCell->content.length() < state->subjectInsertPos)
			state->subjectCell->content += label;
		else
			state->subjectCell->content.replace(state->subjectInsertPos, label.length(), label);

		updateCellContentTexture(renderer, state->FONT, state->fontColor, state->subjectCell);
	}
	
}

void ExprMode::handleKeydownEvent(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state)
{
	switch (e->key.keysym.scancode) {
	case SDL_SCANCODE_RETURN:
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

void ExprMode::handleMouseButtonDown(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state) {
	switch (e->button.button) {
	case SDL_BUTTON_LEFT:
		break;

	case SDL_BUTTON_RIGHT:
		break;

	case SDL_BUTTON_MIDDLE:
		break;
	}

}

void ExprMode::handleTextInput(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state) {

}