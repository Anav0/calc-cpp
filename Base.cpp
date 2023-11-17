#include "Base.h";

void Cell::updateContent(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, int padding[4], std::string* newContent) {

	if (*newContent == "") return;
	//Note(Igor): Rendering text like this is hugely inefficient. We allocate whenever text changes
	SDL_Surface* text = TTF_RenderText_Blended(font, newContent->c_str(), color);

	if (text == NULL) {
		contentTexture = NULL;
		return;
	}

	contentRect.h = text->h;
	contentRect.w = text->w;

	left(&rect, &contentRect, padding);
	contentTexture = SDL_CreateTextureFromSurface(renderer, text);
}

void Cell::showErrorMessage(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, int padding[4]) {

	if (Err == Ok) return;

	std::string errorMsg;

	switch (Err) {
	case NaN:
		errorMsg = "NaN";
		break;
	case SelfReference:
		errorMsg = "Cell cannot reference itself";
		break;
	}

	Cell::updateContent(renderer, font, color, padding, &errorMsg);
}

void Cell::showFormula(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, int padding[4]) {
	updateContent(renderer, font, color, padding, &formula);
}

void Cell::showEvaluation(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, int padding[4]) {
	updateContent(renderer, font, color, padding, &content);
}

