#include "Mode.h"

#include "ProgramState.h"

void Mode::handleEvents(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state)
{
	switch (e->type) {
		case SDL_QUIT:
			state->SHOULD_QUIT = true;
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
	state->MOUSE_POS.x = e->motion.x;
	state->MOUSE_POS.y = e->motion.y;
}

void Mode::handleWindowEvent(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state) {
	switch (e->window.event) {
		case SDL_WINDOWEVENT_RESIZED:
			SDL_Log("Window %d resized to %dx%d", e->window.windowID, e->window.data1, e->window.data2);
			state->SCREEN_WIDTH = e->window.data1;
			state->SCREEN_HEIGHT = e->window.data2;

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
			state->CURRENT_MODE = View;
			break;
		case SDL_SCANCODE_RETURN:
			state->CURRENT_MODE = View;
			break;
		case SDLK_BACKSPACE:
			state->SELECTED_CELL->content[strlen(state->SELECTED_CELL->content) - 1] = '\0';
			updateCellContentTexture(renderer, state->FONT, state->FONT_COLOR, state->SELECTED_CELL);
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
	state->CURRENT_MODE = View;
}

void EditMode::handleTextInput(SDL_Renderer* renderer, SDL_Event* e, ProgramState* state){
	if (state->SELECTED_CELL != NULL) {
		if (SDL_strlen(state->SELECTED_CELL->content) + SDL_strlen(e->text.text) < MAX_TEXT_LEN) {
			SDL_strlcat(state->SELECTED_CELL->content, e->text.text, sizeof(state->SELECTED_CELL->content));

			updateCellContentTexture(renderer, state->FONT, state->FONT_COLOR, state->SELECTED_CELL);
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
		state->CURRENT_MODE = Edit;
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

			for (Cell& cell : state->CELLS) {
				if (SDL_PointInRect(&state->MOUSE_POS, &cell.rect)) {
					state->SELECTED_CELL = &cell;
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
	int numberOfColumns = state->COLUMNS.size();
	int numberOfRows = state->ROWS.size();

	int index;
	for (index = 0; index < state->CELLS.size(); index++) {
		if (cell == &state->CELLS[index]) {
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

	if (newIndex < 0 || newIndex > state->CELLS.size()) return NULL;

	return &state->CELLS[newIndex];
}

void ViewMode::navigate(SDL_Renderer* renderer, Direction direction, ProgramState* state) {
	switch (state->CURRENT_MODE) {
	case View:
	{
		if (state->SELECTED_CELL == NULL) return;

		Cell* newCell = getCellToThe(state->SELECTED_CELL, direction, state);

		if (newCell != NULL) {
			state->SELECTED_CELL = newCell;
		}
		break;
	}
	case Edit:
		//Move caret
		break;
	}
}