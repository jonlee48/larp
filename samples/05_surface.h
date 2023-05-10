#include <SDL2/SDL.h>
#pragma once

/**
 * Init anything before main loop
 * Start up SDL and create new window
 */
bool init(void);

/**
 * Frees media and shuts down SDL
 */
void end(void);

/**
 * Load an individual image
 */
SDL_Texture *load_texture(std::string path);
