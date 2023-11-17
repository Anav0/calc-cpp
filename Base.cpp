#include "Base.h";

void Cell::updateContentTexture(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color) {
	
	SDL_Surface* text = TTF_RenderText_Blended(font, content.c_str(), color);

	if (text == NULL) {
		contentTexture = NULL;
		return;
	}

	contentRect = { rect.x, rect.y + ((rect.h - text->h) / 2), text->w, text->h };
	contentTexture = SDL_CreateTextureFromSurface(renderer, text);
}