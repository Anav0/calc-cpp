#include "Gui.h"

SDL_Renderer* Gui::renderer;
TTF_Font* Gui::defaultFont;
SDL_Point Gui::mousePos;
bool Gui::clicked;
UiGroup Gui::currentGroup;

void Gui::init(SDL_Renderer* renderer, TTF_Font* font)
{
	Gui::defaultFont = font;
	Gui::renderer = renderer;
}

void Gui::drawText(SDL_Color color, std::string content)
{
	Gui::drawText(color, content, Gui::defaultFont);
}

void Gui::drawText(SDL_Color color, std::string content, TTF_Font* font)
{
	int x, y;
	Gui::currentGroup.getLastXandY(&x, &y);

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

	auto texture = SDL_CreateTextureFromSurface(Gui::renderer, text);

	SDL_SetRenderDrawColor(Gui::renderer, 66, 211, 248, 1);
	SDL_RenderFillRect(Gui::renderer, &containerRect);
	SDL_RenderCopy(Gui::renderer, texture, NULL, &contentRect);

	Gui::currentGroup.children.push_back(containerRect);
}

bool Gui::drawBtn(SDL_Color color, std::string content)
{
	int x, y;
	Gui::currentGroup.getLastXandY(&x, &y);

	SDL_Surface* text = TTF_RenderText_Blended(Gui::defaultFont, content.c_str(), color);

	SDL_Rect btnRect{};
	SDL_Rect contentRect{};

	btnRect.x = x;
	btnRect.y = y;

	btnRect.h = text->h + 10;
	btnRect.w = text->w + 10;

	contentRect.h = text->h;
	contentRect.w = text->w;

	int padding[4] = { 5, 0, 5, 0 };
	center(&btnRect, &contentRect);

	auto texture = SDL_CreateTextureFromSurface(Gui::renderer, text);

	//TODO(Igor): get colors from config
	if (SDL_PointInRect(&Gui::mousePos, &btnRect))
		SDL_SetRenderDrawColor(Gui::renderer, 136, 136, 136, 1);
	else
		SDL_SetRenderDrawColor(Gui::renderer, 54, 183, 131, 1);

	SDL_RenderFillRect(Gui::renderer, &btnRect);
	SDL_RenderCopy(Gui::renderer, texture, NULL, &contentRect);

	Gui::currentGroup.children.push_back(btnRect);

	return Gui::clicked && SDL_PointInRect(&Gui::mousePos, &btnRect);
}

void Gui::events(SDL_Event* e)
{
	switch (e->type) {

	case SDL_MOUSEMOTION:
		Gui::mousePos.x = e->motion.x;
		Gui::mousePos.y = e->motion.y;
		break;

	case SDL_MOUSEBUTTONDOWN:
		Gui::clicked = true;
		break;
	}
}

void Gui::endOfLoop() {
	Gui::clicked = false;
	Gui::currentGroup = {};
}

void Gui::startGroup(int x, int y, bool isVertical, int gaps) {
	Gui::currentGroup = UiGroup{ x, y, gaps, isVertical };
}

void Gui::endGroup() {
	Gui::currentGroup = {};
}