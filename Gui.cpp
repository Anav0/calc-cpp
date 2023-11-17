#include "Gui.h"

SDL_Renderer* Gui::renderer;
TTF_Font* Gui::defaultFont;
SDL_Point Gui::mousePos;
bool Gui::clicked;
int Gui::activeElementId;
UiGroup Gui::currentGroup;
SDL_Event* Gui::lastEvent;
int Gui::caretPos;

void Gui::init(SDL_Renderer* renderer, TTF_Font* font)
{
	Gui::activeElementId = -1;
	Gui::defaultFont = font;
	Gui::renderer = renderer;
}

void Gui::drawText(SDL_Color* color, std::string* content, int padding[4], int width, int height)
{
	Gui::drawText(color, content, Gui::defaultFont, padding, width, height);
}

void Gui::drawText(SDL_Color* color, std::string* content, TTF_Font* font, int padding[4], int width, int height)
{
	int x, y;
	Gui::currentGroup.getLastXandY(&x, &y);

	SDL_Surface* text = TTF_RenderText_Blended(font, content->c_str(), *color);

	SDL_Rect contentRect{};
	contentRect.h = text->h;
	contentRect.w = text->w;

	SDL_Rect containerRect{};
	containerRect.x = x;
	containerRect.y = y;
	containerRect.h = text->h;
	containerRect.w = text->w;


	if (width > 0)
		containerRect.w = width;
	if (height > 0)
		containerRect.h = height;

	center(&containerRect, &contentRect); //Note(Igor): Center for now, later use padding and style options

	auto texture = SDL_CreateTextureFromSurface(Gui::renderer, text);

	SDL_SetRenderDrawColor(Gui::renderer, 66, 211, 248, 1);
	SDL_RenderFillRect(Gui::renderer, &containerRect);
	SDL_RenderCopy(Gui::renderer, texture, NULL, &contentRect);

	Gui::currentGroup.children.push_back(containerRect);
}

bool Gui::drawInput(int id, SDL_Color color, std::string* content, int padding[4], int width, int height) {

	int x, y;
	bool result = false;
	Gui::currentGroup.getLastXandY(&x, &y);

	SDL_Surface* text = TTF_RenderText_Blended(Gui::defaultFont, content->c_str(), color);
	auto isActive = Gui::activeElementId == id;

	if (isActive && Gui::lastEvent->type == SDL_TEXTINPUT) {
		content->insert(Gui::caretPos, Gui::lastEvent->text.text);
		Gui::caretPos += 1;
		result = true;
	}

	SDL_Rect inputRect{};
	SDL_Rect contentRect{};

	inputRect.x = x;
	inputRect.y = y;

	if (height > 0)
		inputRect.h = height;
	else if (text != NULL)
		inputRect.h = text->h + padding[2];

	if (width > 0)
		inputRect.w = width;
	else if (text != NULL)
		inputRect.w = text->w + padding[0];

	if (text != NULL && !content->empty()) {
		contentRect.h = text->h;
		contentRect.w = text->w;
	}

	centerY(&inputRect, &contentRect);
	left(&inputRect, &contentRect, padding[0]);

	if (isActive) {
		SDL_SetRenderDrawColor(Gui::renderer, 255, 71, 61, 1); //Light red caret
		if (Gui::caretPos < 0) Gui::caretPos = 0;
		if (Gui::caretPos >= content->length()) Gui::caretPos = content->length();
		drawCaret(2, &inputRect, content, Gui::caretPos, &color, padding[0]);
	}

	if (!isActive && SDL_PointInRect(&Gui::mousePos, &inputRect))
		SDL_SetRenderDrawColor(Gui::renderer, 79, 177, 206, 1); //Blue
	else
		SDL_SetRenderDrawColor(Gui::renderer, 255, 255, 255, 1); //White

	auto texture = SDL_CreateTextureFromSurface(Gui::renderer, text);
	if (!isActive && SDL_PointInRect(&Gui::mousePos, &inputRect))
		SDL_RenderFillRect(renderer, &inputRect);
	else
		SDL_RenderDrawRect(renderer, &inputRect);
	SDL_RenderCopy(Gui::renderer, texture, NULL, &contentRect);

	Gui::currentGroup.children.push_back(inputRect);

	if (Gui::clicked && SDL_PointInRect(&Gui::mousePos, &inputRect)) {
		Gui::activeElementId = id;
	}

	return result;
}

void Gui::drawCaret(int thickness, SDL_Rect* inputRect, std::string* content, int caretPos, SDL_Color* color, int padding) {
	SDL_Rect caretRect{};
	caretRect.w = thickness;
	caretRect.h = inputRect->h - 15;

	moveCaret(&caretRect, inputRect, caretPos, content, padding);

	SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
	SDL_RenderFillRect(renderer, &caretRect);
}

void Gui::moveCaret(SDL_Rect* caret, SDL_Rect* parent, int pos, std::string* content, int padding) {
	centerY(parent, caret);
	caret->x = parent->x + padding;

	if (pos <= 0 || pos > content->length())
		return;

	std::string contentToJumpOver = content->substr(0, pos);

	int jumpOverW, jumpOverH;

	if (TTF_SizeText(Gui::defaultFont, contentToJumpOver.c_str(), &jumpOverW, &jumpOverH)) {
		printf("Failed to determing size of individual char\n");
		return;
	}

	caret->x += jumpOverW;
}

bool Gui::drawBtn(SDL_Color* color, std::string* content)
{
	int x, y;
	Gui::currentGroup.getLastXandY(&x, &y);

	SDL_Surface* text = TTF_RenderText_Blended(Gui::defaultFont, content->c_str(), *color);

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
	Gui::lastEvent = e;
	switch (e->type) {

	case SDL_MOUSEMOTION:
		Gui::mousePos.x = e->motion.x;
		Gui::mousePos.y = e->motion.y;
		break;

	case SDL_KEYDOWN:
		Gui::handleKeydownEvent(e);
		break;

	case SDL_MOUSEBUTTONDOWN:
		Gui::activeElementId = -1;
		Gui::caretPos = 0; //TODO(Igor): move caret to clicked character pos
		Gui::clicked = true;
		break;
	}
}

void Gui::handleKeydownEvent(SDL_Event* e) {
	switch (e->key.keysym.scancode) {
	case SDL_SCANCODE_LEFT:
		Gui::caretPos -= 1;
		break;
	case SDL_SCANCODE_RIGHT:
		Gui::caretPos += 1;
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