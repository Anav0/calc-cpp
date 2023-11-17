#pragma once

#include <stdio.h>
#include <SDL.h>

#include "Base.h"
#include "Eval.h"

struct ProgramState;

class Mode
{
protected:
	virtual void handleKeydownEvent(SDL_Renderer*, SDL_Event*, ProgramState*);
	virtual void handleMouseButtonDown(SDL_Renderer*, SDL_Event*, ProgramState*);
	virtual void handleTextInput(SDL_Renderer*, SDL_Event*, ProgramState*);
	virtual void handleMouseMotion(SDL_Renderer*, SDL_Event*, ProgramState*);
	virtual void handleWindowEvent(SDL_Renderer*, SDL_Event*, ProgramState*);
	
public:
	ModeType type;
	SDL_Color color;

	Mode() {
		color = { 255, 206, 69, 0 };
	}

	void handleEvents(SDL_Renderer*, SDL_Event*, ProgramState*);
	virtual void onSwitchTo(SDL_Renderer*, ProgramState*);
};

class EditMode : public Mode {
private:
	void moveToExprMode(SDL_Renderer*, ProgramState*, Direction);
	
public:
	EditMode() {
		type = Edit;
		color = { 206, 152, 250, 0 };
	}

	// Inherited via Mode
	virtual void handleKeydownEvent(SDL_Renderer*, SDL_Event*, ProgramState*) override;
	virtual void handleMouseButtonDown(SDL_Renderer*, SDL_Event*, ProgramState*) override;
	virtual void handleTextInput(SDL_Renderer*, SDL_Event*, ProgramState*) override;
	virtual void onSwitchTo(SDL_Renderer*, ProgramState*) override;
};

class ViewMode : public Mode {
private:
	void navigate(SDL_Renderer*, Direction, ProgramState*);
public:
	ViewMode() {
		type = View;
	}

	// Inherited via Mode
	virtual void handleKeydownEvent(SDL_Renderer*, SDL_Event*, ProgramState*) override;
	virtual void handleMouseButtonDown(SDL_Renderer*, SDL_Event*, ProgramState*) override;
	virtual void handleTextInput(SDL_Renderer*, SDL_Event*, ProgramState*) override;
};

class ExprMode : public Mode {
private:
	void navigate(SDL_Renderer*, Direction, ProgramState*);
	void addOperator(SDL_Renderer*, char, ProgramState*);

public:
	ExprMode() {
		type = Expr;
		color = { 115, 202, 246, 0 };
	}

	// Inherited via Mode
	virtual void handleKeydownEvent(SDL_Renderer*, SDL_Event*, ProgramState*) override;
	virtual void onSwitchTo(SDL_Renderer*, ProgramState*) override;
};