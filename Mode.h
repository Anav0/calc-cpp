#pragma once

#include <stdio.h>

#include <SDL.h>

#include "Base.h"
#include "tinyexpr.h"

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

	void handleEvents(SDL_Renderer*, SDL_Event*, ProgramState*);
};

class EditMode : public Mode {
	
public:
	EditMode() {
		type = Edit;
	}

	// Inherited via Mode
	virtual void handleKeydownEvent(SDL_Renderer*, SDL_Event*, ProgramState*) override;
	virtual void handleMouseButtonDown(SDL_Renderer*, SDL_Event*, ProgramState*) override;
	virtual void handleTextInput(SDL_Renderer*, SDL_Event*, ProgramState*) override;
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
};

class ExprMode : public Mode {
private:
	void navigate(SDL_Renderer*, Direction, ProgramState*);
	void addOperator(SDL_Renderer*, char, ProgramState*);
	void evaluate(SDL_Renderer*, ProgramState*);
public:
	ExprMode() {
		type = Expr;
	}

	// Inherited via Mode
	virtual void handleKeydownEvent(SDL_Renderer*, SDL_Event*, ProgramState*) override;
	virtual void handleMouseButtonDown(SDL_Renderer*, SDL_Event*, ProgramState*) override;
	virtual void handleTextInput(SDL_Renderer*, SDL_Event*, ProgramState*) override;
};