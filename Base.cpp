#include "Base.h";

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

	SDL_Surface* text = TTF_RenderText_Blended(font, errorMsg.c_str(), color);

	contentRect.h = text->h;
	contentRect.w = text->w;

	left(&rect, &contentRect, padding);
	contentTexture = SDL_CreateTextureFromSurface(renderer, text);
}

void Cell::showFormula(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, int padding[4]) {

	if (formula == "") return;

	SDL_Surface* text = TTF_RenderText_Blended(font, formula.c_str(), color);

	contentRect.h = text->h;
	contentRect.w = text->w;

	left(&rect, &contentRect, padding);
	contentTexture = SDL_CreateTextureFromSurface(renderer, text);
}

void Cell::showEvaluation(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, int padding[4]) {

	SDL_Surface* text = TTF_RenderText_Blended(font, content.c_str(), color);

	if (text == NULL) {
		contentTexture = NULL;
		return;
	}

	contentRect.h = text->h;
	contentRect.w = text->w;

	left(&rect, &contentRect, padding);
	contentTexture = SDL_CreateTextureFromSurface(renderer, text);
}

void drawText(SDL_Renderer* renderer, int x, int y, SDL_Color color, std::string content, TTF_Font* font)
{
	SDL_Surface* text = TTF_RenderText_Blended(font, content.c_str(), color);

	SDL_Rect containerRect{};
	SDL_Rect contentRect{};

	containerRect.x = x;
	containerRect.y = y;
	containerRect.h = text->h + 20;
	containerRect.w = text->w + 20;

	contentRect.h = text->h;
	contentRect.w = text->w;

	int padding[4] = { 10, 0, 10,0 };
	center(&containerRect, &contentRect);

	auto texture = SDL_CreateTextureFromSurface(renderer, text);

	SDL_SetRenderDrawColor(renderer, 66, 211, 248, 1);
	SDL_RenderFillRect(renderer, &containerRect);
	SDL_RenderCopy(renderer, texture, NULL, &contentRect);
}
