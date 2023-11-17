#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <string>
#include "Align.h"

#include <vector>
#include <string>

struct UiGroup {
	int x, y, gaps;
	bool isVertical;
	std::vector<SDL_Rect> children;

	void getLastXandY(int* sx, int* sy) {
		if (children.size() == 0) {
			*sx = x;
			*sy = y;
			return;
		}

		auto lastElement = children.back();
		*sx = lastElement.x;
		*sy = lastElement.y;

		if (isVertical)
			*sy += lastElement.h + gaps;
		else
			*sx += lastElement.w + gaps;
	}

	int getTotalWidth() {
		if (isVertical) {
			int widest = -1;
			for (size_t i = 0; i < children.size(); i++) {
				if (children[i].w > widest) {
					widest = children[i].w;
				}
			}
			return widest;
		}

		int sum = 0;
		for (size_t i = 0; i < children.size(); i++)
			sum += children[i].w;

		return sum + (gaps * (children.size() - 1));
	}

	int getTotalHeight() {
		if (!isVertical) {
			int highest = -1;
			for (size_t i = 0; i < children.size(); i++) {
				if (children[i].h > highest) {
					highest = children[i].h;
				}
			}
			return highest;
		}

		int sum = 0;
		for (size_t i = 0; i < children.size(); i++)
			sum += children[i].h;

		return sum + (gaps * (children.size() - 1));
	}
};

class Gui {
private:
	static SDL_Renderer* renderer;
	static TTF_Font* defaultFont;
	static SDL_Point mousePos;
	static bool clicked;
	static UiGroup currentGroup;
	static SDL_Event* lastEvent;
	static int caretPos;
	Gui() {};

public:
	static int activeElementId;

	static void init(SDL_Renderer*, TTF_Font*);
	static void endOfLoop();

	static void startGroup(int x, int y, bool isVertical, int gaps);
	static void endGroup();

	static void drawText(SDL_Color*, std::string*, TTF_Font*, int padding[4], int width = -1, int height = -1);
	static void drawText(SDL_Color*, std::string*, int padding[4], int width = -1, int height = -1);
	static bool drawBtn(SDL_Color*, std::string*);
	static bool drawInput(int id, SDL_Color, std::string*, int padding[4], int width = -1, int height = -1);

	static void drawCaret(int thickness, SDL_Rect* inputRect, std::string* content, int caretPos, SDL_Color* color, int padding);
	static void moveCaret(SDL_Rect* caret, SDL_Rect* parent, int pos, std::string* content, int padding);

	static void handleKeydownEvent(SDL_Event*);
	static void events(SDL_Event* mostRecentEvent);
};