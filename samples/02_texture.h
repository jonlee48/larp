#include <SDL2/SDL.h>
#pragma once

/**
 * Init anything before main loop
 * Start up SDL and create new window
 */
bool init(void);

/**
 * Loads media
 */
bool load_media(void);

/**
 * Frees media and shuts down SDL
 */
void close(void);

/**
 * Load an individual image
 */
SDL_Surface *load_surface(std::string path);
