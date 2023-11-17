#pragma once

#include <SDL.h>

inline void centerX(SDL_Rect* parent, SDL_Rect* child)
{
	child->x = parent->x + ((parent->w - child->w) / 2);

}

inline void centerY(SDL_Rect* parent, SDL_Rect* child)
{
	child->y = parent->y + ((parent->h - child->h) / 2);
}


inline void center(SDL_Rect* parent, SDL_Rect* child)
{
	child->x = parent->x + ((parent->w - child->w) / 2);
	child->y = parent->y + ((parent->h - child->h) / 2);
}

inline void left(SDL_Rect* parent, SDL_Rect* child, int padding[4])
{
	child->x = parent->x + padding[0];
	child->y = parent->y + padding[2] + ((parent->h - child->h) / 2);
}

inline void left2(SDL_Rect* parent, SDL_Rect* child, int padding)
{
	child->x = parent->x + padding;
}