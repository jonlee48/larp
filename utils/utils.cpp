#define _USE_MATH_DEFINES

#include "utils.h"
#include "constants.h"
#include <SDL2/SDL.h>
#include <cmath>

float radians(float degrees) {
    return degrees * M_PI / 180.0;
}

float degrees(float radians) {
    return radians * 180.0 / M_PI;
}

int comparefloats(float x, float y, float epsilon) {
    if (fabs(x-y) < epsilon) {
        return 0;
    }
    else if (x < y) {
        return -1;
    }
    else {
        return 1;
    }
}

void SetPixel(SDL_Surface *surface, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_LockSurface(surface);
    Uint32 *buffer = (Uint32*) surface->pixels;
    int offset = y * surface->w + x;
    Uint32 pixel = SDL_MapRGBA(surface->format, r, g, b, a);
    buffer[offset] = pixel;
    SDL_UnlockSurface(surface);
}

void GetPixel(SDL_Surface *surface, int x, int y, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a) {
    SDL_LockSurface(surface);
    Uint32 *buffer = (Uint32*) surface->pixels;
    int offset = y * surface->w + x;
    Uint32 pixel = buffer[offset];
    SDL_GetRGBA(pixel, surface->format, r, g, b, a); 
    SDL_UnlockSurface(surface);
}