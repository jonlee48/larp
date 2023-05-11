#pragma once
#include <SDL2/SDL.h>

double radians(double degrees);
double degrees(double radians);
int comparedoubles(double x, double y, double epsilon);

void SetPixel(SDL_Surface *surface, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void GetPixel(SDL_Surface *surface, int x, int y, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a);