#include "Base.h";

void Cell::updateContentTexture(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, int padding[4]) {
	
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