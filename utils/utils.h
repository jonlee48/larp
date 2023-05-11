#pragma once
#include <SDL2/SDL.h>

float radians(float degrees);
float degrees(float radians);
int comparefloats(float x, float y, float epsilon);

void SetPixel(SDL_Surface *surface, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void GetPixel(SDL_Surface *surface, int x, int y, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a);