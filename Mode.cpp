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
		case SDLK_BACKSPACE:
			state->selectedCell->content[strlen(state->selectedCell->content) - 1] = '\0';
			updateCellContentTexture(renderer, state->FONT, state->fontColor, state->selectedCell);
			break;
		case SDL_SCANCODE_DOWN:
			break;
		case SDL_SCANCODE_UP:
			break;
		case SDL_SCANCODE_LEFT:
			break;
		case SDL_SCANCODE_RIGHT:
			break;
	}
}

void EditMode::handleMouseButtonDown(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state){
	state->currentMode = View;
}

void EditMode::handleTextInput(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state){
	if (state->selectedCell != NULL) {
		if (SDL_strlen(state->selectedCell->content) + SDL_strlen(e->text.text) < MAX_TEXT_LEN) {
			SDL_strlcat(state->selectedCell->content, e->text.text, sizeof(state->selectedCell->content));

			updateCellContentTexture(renderer, state->FONT, state->fontColor, state->selectedCell);
		}
		else {
			printf("Input is too big!\n");
		}
	}
}

// ===============
// == VIEW MODE ==
// ===============

void ViewMode::handleKeydownEvent(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state)
{
	switch (e->key.keysym.scancode) {
	case SDL_SCANCODE_RETURN:
		state->currentMode = Edit;
		break;
	case SDL_SCANCODE_DOWN:
		ViewMode::navigate(renderer, Direction::Down, state);
		break;
	case SDL_SCANCODE_UP:
		ViewMode::navigate(renderer, Direction::Up, state);
		break;
	case SDL_SCANCODE_LEFT:
		ViewMode::navigate(renderer, Direction::Left, state);
		break;
	case SDL_SCANCODE_RIGHT:
		ViewMode::navigate(renderer, Direction::Right, state);
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

		case SDL_BUTTON_RIGHT:
			break;

		case SDL_BUTTON_MIDDLE:
			break;
	}

}



void ViewMode::handleTextInput(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state){}

Cell* ViewMode::getCellToThe(Cell* cell, Direction direction, ProgramState* state) {
	int numberOfColumns = state->columns.size();
	int numberOfRows = state->rows.size();

	int index;
	for (index = 0; index < state->cells.size(); index++) {
		if (cell == &state->cells[index]) {
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

	if (newIndex < 0 || newIndex > state->cells.size()) return NULL;

	return &state->cells[newIndex];
}

void ViewMode::navigate(SDL_Renderer* renderer, Direction direction, ProgramState* state) {
	switch (state->currentMode) {
	case View:
	{
		if (state->selectedCell == NULL) return;

		Cell* newCell = getCellToThe(state->selectedCell, direction, state);

		if (newCell != NULL) {
			state->selectedCell = newCell;
		}
		break;
	}
	case Edit:
		//Move caret
		break;
	}
}